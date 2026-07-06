// OpenRip M0 — optical spin sensing, presence-gated (SPEC.md §4, §8).
//
// Polls a TCRT5000 IR reflective sensor aimed at the winder launcher's
// factory encoder disc (software Schmitt trigger, thresholds in config.h)
// and a plunger microswitch that reports bey presence. Launch detection is
// a state machine:
//
//   IDLE ──plunger high──▶ ARMED ──first pulse──▶ RIPPING ──plunger falls──▶ report
//
// The plunger falling edge timestamps the release; the record carries both
// peak RPM (min interval) and release RPM (last interval before the edge).
// Pulses are ignored while the plunger is low, so winding/re-insertion
// can't fake a launch. The 250ms pulse timeout survives only as a fallback
// for a missed release edge. Send 'r' over serial to toggle raw mode
// (streams "micros,adc" for signal characterization). BLE is a no-op
// unless built with -DOPENRIP_ENABLE_BLE.

#include <Arduino.h>

#include "config.h"
#include "launch_record.h"
#include "ble_service.h"

#ifdef OPENRIP_ENABLE_SLEEP
#include <esp_sleep.h>
#endif

// --- Launch state machine -------------------------------------------------------

enum class LaunchState : uint8_t { IDLE, ARMED, RIPPING };

static LaunchState g_state = LaunchState::IDLE;

static const char *stateName(LaunchState s) {
    switch (s) {
        case LaunchState::IDLE:    return "IDLE";
        case LaunchState::ARMED:   return "ARMED";
        default:                   return "RIPPING";
    }
}

static void setState(LaunchState s) {
    if (s == g_state) return;
    g_state = s;
    Serial.printf("# state: %s\n", stateName(s));
}

// --- Plunger (bey presence) -------------------------------------------------------

static bool     g_beyPresent        = false; // debounced state
static bool     g_plungerRaw        = false;
static uint32_t g_plungerRawSinceMs = 0;
static uint32_t g_plungerRawEdgeUs  = 0;     // micros() of the raw edge under debounce

// Debounced read. Returns true when the stable state just changed;
// g_plungerRawEdgeUs then holds the timestamp of the raw edge that started
// the transition (closer to the physical event than the debounce-confirm
// time).
static bool updatePlunger() {
    const bool raw = (digitalRead(PIN_PLUNGER) == LOW); // pressed = bey present
    if (raw != g_plungerRaw) {
        g_plungerRaw        = raw;
        g_plungerRawSinceMs = millis();
        g_plungerRawEdgeUs  = micros();
    }
    if (raw != g_beyPresent && millis() - g_plungerRawSinceMs >= PLUNGER_DEBOUNCE_MS) {
        g_beyPresent = raw;
        return true;
    }
    return false;
}

// --- Pulse capture ------------------------------------------------------------

static uint32_t g_pulseCount     = 0;
static uint32_t g_firstPulseUs   = 0;
static uint32_t g_lastPulseUs    = 0;
static uint32_t g_minIntervalUs  = UINT32_MAX; // shortest interval = peak RPM
static uint32_t g_lastIntervalUs = 0;          // latest interval = instantaneous/release RPM
static bool     g_schmittArmed   = true;       // waiting for a LOW crossing
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
// (white segment) pulls the reading LOW; falling below SENSOR_THRESH_LOW is
// a pulse, and the detector re-arms above SENSOR_THRESH_HIGH. Pulses only
// register while a bey is present (SPEC §4) — the Schmitt state still
// tracks so the first armed pulse is clean.
static void sampleSensor() {
    const uint32_t now = micros();
    const int adc = analogRead(PIN_SENSOR);

    if (g_rawMode) {
        // Serial is the throughput bottleneck (~700 lines/s at 115200) —
        // that is still "max rate" for characterization purposes.
        Serial.printf("%lu,%d\n", (unsigned long)now, adc);
    }

    if (g_schmittArmed && adc < SENSOR_THRESH_LOW) {
        if (g_beyPresent) registerPulse(now);
        g_schmittArmed = false;
    } else if (!g_schmittArmed && adc > SENSOR_THRESH_HIGH) {
        g_schmittArmed = true;
    }
}

// --- Helpers ------------------------------------------------------------------

// Hook RPM from a pulse interval: encoder RPM scaled by the gear ratio
// (GEAR_RATIO, config.h — confirmed 1.0: the encoder disc sits on the hook
// shaft).
static uint32_t rpmFromIntervalUs(uint32_t intervalUs) {
    if (intervalUs == 0) return 0;
    const uint64_t encoderRpm = 60000000ULL / ((uint64_t)intervalUs * PULSES_PER_REV);
    return (uint32_t)((float)encoderRpm * GEAR_RATIO + 0.5f);
}

static void resetCapture() {
    g_pulseCount     = 0;
    g_firstPulseUs   = 0;
    g_lastPulseUs    = 0;
    g_minIntervalUs  = UINT32_MAX;
    g_lastIntervalUs = 0;
}

static void reportLaunch(bool viaReleaseEdge) {
    static uint16_t s_seq = 0;

    const uint32_t durationMs = (g_lastPulseUs - g_firstPulseUs) / 1000;
    const uint32_t peakRpm    = rpmFromIntervalUs(g_minIntervalUs);
    const uint32_t releaseRpm = rpmFromIntervalUs(g_lastIntervalUs);
    // pulseCount-1 intervals over the launch window
    const uint32_t avgRpm = durationMs > 0
        ? rpmFromIntervalUs((g_lastPulseUs - g_firstPulseUs) / (g_pulseCount - 1))
        : peakRpm;

    launch_record_t rec = {};
    rec.seq             = s_seq++;
    rec.peak_rpm        = (uint16_t)min(peakRpm, (uint32_t)UINT16_MAX);
    rec.release_rpm     = (uint16_t)min(releaseRpm, (uint32_t)UINT16_MAX);
    rec.rip_duration_ms = (uint16_t)min(durationMs, (uint32_t)UINT16_MAX);
    rec.battery_pct     = 100; // USB-powered on the breadboard; real gauge lands with M1

    Serial.println(F("---- LAUNCH ----"));
    Serial.printf("seq:         %u\n", rec.seq);
    Serial.printf("peak RPM:    %u\n", rec.peak_rpm);
    Serial.printf("release RPM: %u\n", rec.release_rpm);
    Serial.printf("avg RPM:     %lu\n", (unsigned long)avgRpm);
    Serial.printf("duration:    %u ms\n", rec.rip_duration_ms);
    Serial.printf("pulses:      %lu (%d/rev)\n", (unsigned long)g_pulseCount, PULSES_PER_REV);
    Serial.printf("end trigger: %s\n", viaReleaseEdge ? "release edge" : "pulse timeout (fallback)");
    Serial.println(F("----------------"));

    bleNotifyLaunch(rec);
}

// --- Power management (M1) ----------------------------------------------------

#ifdef OPENRIP_ENABLE_SLEEP
// Millis timestamp of the last thing worth staying awake for: boot, a bey
// seated/ripping, or a connected BLE client.
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
    pinMode(PIN_PLUNGER, INPUT_PULLUP);

#ifdef OPENRIP_ENABLE_SLEEP
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    s_lastActivityMs = millis();
#endif

    bleInit();

    Serial.println(F("OpenRip M0 — presence-gated optical spin proof."));
    Serial.println(F("Send 'r' for raw ADC mode. Lock a bey in to arm."));
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

    const bool plungerChanged = updatePlunger();
    sampleSensor();

#ifdef OPENRIP_ENABLE_SLEEP
    if (g_state != LaunchState::IDLE || bleIsConnected()) s_lastActivityMs = millis();
    if (millis() - s_lastActivityMs >= IDLE_SLEEP_TIMEOUT_MS) goToSleep();
#endif

    switch (g_state) {
        case LaunchState::IDLE:
            if (plungerChanged && g_beyPresent) {
                resetCapture();
                setState(LaunchState::ARMED);
            }
            break;

        case LaunchState::ARMED:
            if (plungerChanged && !g_beyPresent) {
                // Bey removed without a rip — disarm quietly.
                setState(LaunchState::IDLE);
            } else if (g_pulseCount > 0) {
                setState(LaunchState::RIPPING);
            }
            break;

        case LaunchState::RIPPING: {
            if (plungerChanged && !g_beyPresent) {
                // RELEASE: the plunger falling edge is the launch end.
                // g_plungerRawEdgeUs is the physical edge time; release RPM
                // comes from the last pulse interval before it.
                if (g_pulseCount >= MIN_PULSES_PER_LAUNCH) {
                    reportLaunch(true);
                } // else: stray pulse while seated — not a launch
                resetCapture();
                setState(LaunchState::IDLE);
                break;
            }
            // Fallback: release edge missed (or bey spun down while still
            // seated) — a pulse gap ends the launch. Bey is still present,
            // so return to ARMED for the next rip.
            if (micros() - g_lastPulseUs >= LAUNCH_END_TIMEOUT_MS * 1000UL) {
                if (g_pulseCount >= MIN_PULSES_PER_LAUNCH) {
                    reportLaunch(false);
                }
                resetCapture();
                setState(LaunchState::ARMED);
                break;
            }
            // Still ripping: push live RPM for training mode (no-op without BLE).
            static uint32_t s_lastLiveMs = 0;
            const uint32_t nowMs = millis();
            if (g_lastIntervalUs > 0 && nowMs - s_lastLiveMs >= LIVE_RPM_PERIOD_MS) {
                s_lastLiveMs = nowMs;
                bleUpdateLiveRpm((uint16_t)min(rpmFromIntervalUs(g_lastIntervalUs), (uint32_t)UINT16_MAX));
            }
            break;
        }
    }
}
