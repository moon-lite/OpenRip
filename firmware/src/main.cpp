// OpenRip M0 — optical spin sensing (SPEC.md §8, M0b).
//
// Polls a QRE1113 IR reflective sensor over ADC and turns reflectivity
// changes into pulse timestamps via a software Schmitt trigger (thresholds
// in config.h). Computes instantaneous and peak RPM from pulse intervals,
// detects launch end after LAUNCH_END_TIMEOUT_MS of silence, and prints a
// launch summary over serial. Send 'r' over serial to toggle raw mode,
// which streams "micros,adc" samples for M0a/M0b signal characterization.
// BLE is a no-op unless built with -DOPENRIP_ENABLE_BLE.

#include <Arduino.h>

#include "config.h"
#include "launch_record.h"
#include "ble_service.h"

#ifdef OPENRIP_ENABLE_SLEEP
#include <esp_sleep.h>
#endif

// --- Pulse capture ------------------------------------------------------------

static uint32_t g_pulseCount     = 0;
static uint32_t g_firstPulseUs   = 0;
static uint32_t g_lastPulseUs    = 0;
static uint32_t g_minIntervalUs  = UINT32_MAX; // shortest interval = peak RPM
static uint32_t g_lastIntervalUs = 0;          // latest interval = instantaneous RPM
static bool     g_armed          = true;       // Schmitt state: waiting for a LOW crossing
static bool     g_rawMode        = false;

static void registerPulse(uint32_t now) {
    if (g_pulseCount == 0) {
        g_firstPulseUs = now;
        g_lastPulseUs  = now;
        g_pulseCount   = 1;
        return;
    }
    const uint32_t interval = now - g_lastPulseUs;
    // Glitch filter: noise spikes are dropped without moving the last-pulse
    // anchor, so the next real pulse still times correctly.
    if (interval >= MIN_PULSE_INTERVAL_US) {
        if (interval < g_minIntervalUs) g_minIntervalUs = interval;
        g_lastIntervalUs = interval;
        g_lastPulseUs    = now;
        g_pulseCount++;
    }
}

// One ADC sample through the software Schmitt trigger. A reflective feature
// pulls the reading LOW (see wiring.md); falling below SENSOR_THRESH_LOW is
// a pulse, and the detector re-arms above SENSOR_THRESH_HIGH.
static void sampleSensor() {
    const uint32_t now = micros();
    const int adc = analogRead(PIN_SENSOR);

    if (g_rawMode) {
        // Serial is the throughput bottleneck (~700 lines/s at 115200) —
        // that is still "max rate" for characterization purposes.
        Serial.printf("%lu,%d\n", (unsigned long)now, adc);
    }

    if (g_armed && adc < SENSOR_THRESH_LOW) {
        registerPulse(now);
        g_armed = false;
    } else if (!g_armed && adc > SENSOR_THRESH_HIGH) {
        g_armed = true;
    }
}

// --- Helpers ------------------------------------------------------------------

static uint32_t rpmFromIntervalUs(uint32_t intervalUs) {
    if (intervalUs == 0) return 0;
    return (uint32_t)(60000000ULL / ((uint64_t)intervalUs * PULSES_PER_REV));
}

static void resetCapture() {
    g_pulseCount     = 0;
    g_firstPulseUs   = 0;
    g_lastPulseUs    = 0;
    g_minIntervalUs  = UINT32_MAX;
    g_lastIntervalUs = 0;
}

static void reportLaunch() {
    static uint16_t s_seq = 0;

    const uint32_t durationMs = (g_lastPulseUs - g_firstPulseUs) / 1000;
    const uint32_t peakRpm    = rpmFromIntervalUs(g_minIntervalUs);
    // pulseCount-1 intervals over the launch window
    const uint32_t avgRpm = durationMs > 0
        ? rpmFromIntervalUs((g_lastPulseUs - g_firstPulseUs) / (g_pulseCount - 1))
        : peakRpm;

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
    Serial.printf("pulses:     %lu (%d/rev)\n", (unsigned long)g_pulseCount, PULSES_PER_REV);
    Serial.println(F("----------------"));

    bleNotifyLaunch(rec);
}

// --- Power management (M1) ----------------------------------------------------

#ifdef OPENRIP_ENABLE_SLEEP
// Millis timestamp of the last thing worth staying awake for: boot, a spool
// pulse, or a connected BLE client.
static uint32_t s_lastActivityMs = 0;

static void goToSleep() {
    Serial.println(F("Idle — deep sleeping. Press button to wake."));
    Serial.flush();
    // Wake on button press (active low, internal pull-up). C3 deep-sleep GPIO
    // wake is limited to GPIO0–5; PIN_BUTTON must stay in that range.
    // TODO(M1 hardware): confirm the internal pull-up holds through deep
    // sleep on the XIAO; add an external 100k pull-up if it doesn't.
    esp_deep_sleep_enable_gpio_wakeup(1ULL << PIN_BUTTON, ESP_GPIO_WAKEUP_GPIO_LOW);
    esp_deep_sleep_start();
}
#endif

// --- Arduino entry points -------------------------------------------------------

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(200);

    pinMode(PIN_SENSOR, INPUT);
    analogReadResolution(12);

#ifdef OPENRIP_ENABLE_SLEEP
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    s_lastActivityMs = millis();
#endif

    bleInit();

    Serial.println(F("OpenRip M0 — optical spin proof. Send 'r' for raw ADC mode. Waiting for launch..."));
}

void loop() {
    // Serial commands: 'r' toggles the raw ADC stream.
    if (Serial.available()) {
        const char c = (char)Serial.read();
        if (c == 'r' || c == 'R') {
            g_rawMode = !g_rawMode;
            Serial.printf("# raw mode %s (thresholds: low=%d high=%d)\n",
                          g_rawMode ? "ON — us,adc" : "OFF",
                          SENSOR_THRESH_LOW, SENSOR_THRESH_HIGH);
        }
    }

    sampleSensor();

#ifdef OPENRIP_ENABLE_SLEEP
    if (g_pulseCount > 0 || bleIsConnected()) s_lastActivityMs = millis();
    if (millis() - s_lastActivityMs >= IDLE_SLEEP_TIMEOUT_MS) goToSleep();
#endif

    if (g_pulseCount == 0) return;

    const uint32_t sinceLastUs = micros() - g_lastPulseUs;

    if (sinceLastUs >= LAUNCH_END_TIMEOUT_MS * 1000UL) {
        if (g_pulseCount >= MIN_PULSES_PER_LAUNCH) {
            reportLaunch();
        }
        // Fewer pulses than MIN_PULSES_PER_LAUNCH → stray reflection; discard.
        resetCapture();
        return;
    }

    // Still spinning: push live RPM for training mode (no-op without BLE).
    static uint32_t s_lastLiveMs = 0;
    const uint32_t nowMs = millis();
    if (g_lastIntervalUs > 0 && nowMs - s_lastLiveMs >= LIVE_RPM_PERIOD_MS) {
        s_lastLiveMs = nowMs;
        bleUpdateLiveRpm((uint16_t)min(rpmFromIntervalUs(g_lastIntervalUs), (uint32_t)UINT16_MAX));
    }
}
