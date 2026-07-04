// OpenRip M0 — hall sensor RPM proof (SPEC.md §8).
//
// Counts hall pulses on a GPIO interrupt with microsecond timestamps,
// computes instantaneous and peak RPM from pulse intervals, detects launch
// end after LAUNCH_END_TIMEOUT_MS of silence, and prints a launch summary
// over serial. BLE is a no-op unless built with -DOPENRIP_ENABLE_BLE.

#include <Arduino.h>

#include "config.h"
#include "launch_record.h"
#include "ble_service.h"

// --- Pulse capture (shared with ISR) -----------------------------------------

static portMUX_TYPE g_pulseMux = portMUX_INITIALIZER_UNLOCKED;

static volatile uint32_t g_pulseCount    = 0;
static volatile uint32_t g_firstPulseUs  = 0;
static volatile uint32_t g_lastPulseUs   = 0;
static volatile uint32_t g_minIntervalUs = UINT32_MAX; // shortest interval = peak RPM
static volatile uint32_t g_lastIntervalUs = 0;         // latest interval = instantaneous RPM

static void IRAM_ATTR onHallPulse() {
    const uint32_t now = micros();
    portENTER_CRITICAL_ISR(&g_pulseMux);
    if (g_pulseCount == 0) {
        g_firstPulseUs = now;
        g_lastPulseUs  = now;
        g_pulseCount   = 1;
    } else {
        const uint32_t interval = now - g_lastPulseUs;
        // Glitch filter: bounce/EMI spikes are dropped without moving the
        // last-pulse anchor, so the next real pulse still times correctly.
        if (interval >= MIN_PULSE_INTERVAL_US) {
            if (interval < g_minIntervalUs) g_minIntervalUs = interval;
            g_lastIntervalUs = interval;
            g_lastPulseUs    = now;
            g_pulseCount++;
        }
    }
    portEXIT_CRITICAL_ISR(&g_pulseMux);
}

// --- Helpers ------------------------------------------------------------------

static uint32_t rpmFromIntervalUs(uint32_t intervalUs) {
    if (intervalUs == 0) return 0;
    return (uint32_t)(60000000ULL / ((uint64_t)intervalUs * PULSES_PER_REV));
}

static void resetCapture() {
    portENTER_CRITICAL(&g_pulseMux);
    g_pulseCount     = 0;
    g_firstPulseUs   = 0;
    g_lastPulseUs    = 0;
    g_minIntervalUs  = UINT32_MAX;
    g_lastIntervalUs = 0;
    portEXIT_CRITICAL(&g_pulseMux);
}

static void reportLaunch(uint32_t pulses, uint32_t firstUs, uint32_t lastUs, uint32_t minIntervalUs) {
    static uint16_t s_seq = 0;

    const uint32_t durationMs = (lastUs - firstUs) / 1000;
    const uint32_t peakRpm    = rpmFromIntervalUs(minIntervalUs);
    // pulses-1 intervals over the launch window
    const uint32_t avgRpm =
        durationMs > 0 ? rpmFromIntervalUs((lastUs - firstUs) / (pulses - 1)) : peakRpm;

    launch_record_t rec = {};
    rec.seq             = s_seq++;
    rec.peak_rpm        = (uint16_t)min(peakRpm, (uint32_t)UINT16_MAX);
    rec.rip_duration_ms = (uint16_t)min(durationMs, (uint32_t)UINT16_MAX);
    rec.battery_pct     = 100; // USB-powered on the breadboard; real gauge lands with M1

    Serial.println(F("---- LAUNCH ----"));
    Serial.printf("seq:        %u\n", rec.seq);
    Serial.printf("peak RPM:   %u\n", rec.peak_rpm);
    Serial.printf("avg RPM:    %lu\n", (unsigned long)avgRpm);
    Serial.printf("duration:   %u ms\n", rec.rip_duration_ms);
    Serial.printf("pulses:     %lu\n", (unsigned long)pulses);
    Serial.println(F("----------------"));

    bleNotifyLaunch(rec);
}

// --- Arduino entry points -------------------------------------------------------

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(200);

    pinMode(PIN_HALL, HALL_PIN_MODE);
    attachInterrupt(digitalPinToInterrupt(PIN_HALL), onHallPulse, HALL_TRIGGER_EDGE);

    bleInit();

    Serial.println(F("OpenRip M0 — hall RPM proof. Waiting for launch..."));
}

void loop() {
    // Snapshot ISR state
    portENTER_CRITICAL(&g_pulseMux);
    const uint32_t pulses         = g_pulseCount;
    const uint32_t firstUs        = g_firstPulseUs;
    const uint32_t lastUs         = g_lastPulseUs;
    const uint32_t minIntervalUs  = g_minIntervalUs;
    const uint32_t lastIntervalUs = g_lastIntervalUs;
    portEXIT_CRITICAL(&g_pulseMux);

    if (pulses == 0) {
        delay(1);
        return;
    }

    const uint32_t sinceLastUs = micros() - lastUs;

    if (sinceLastUs >= LAUNCH_END_TIMEOUT_MS * 1000UL) {
        if (pulses >= MIN_PULSES_PER_LAUNCH) {
            reportLaunch(pulses, firstUs, lastUs, minIntervalUs);
        }
        // Fewer pulses than MIN_PULSES_PER_LAUNCH → stray magnet pass; discard.
        resetCapture();
        return;
    }

    // Still spinning: push live RPM for training mode (no-op without BLE).
    static uint32_t s_lastLiveMs = 0;
    const uint32_t nowMs = millis();
    if (lastIntervalUs > 0 && nowMs - s_lastLiveMs >= LIVE_RPM_PERIOD_MS) {
        s_lastLiveMs = nowMs;
        bleUpdateLiveRpm((uint16_t)min(rpmFromIntervalUs(lastIntervalUs), (uint32_t)UINT16_MAX));
    }

    delay(1);
}
