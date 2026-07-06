#pragma once

// ---------------------------------------------------------------------------
// OpenRip firmware configuration — pin assignments and tunables.
// Board: Seeed XIAO ESP32-C3. See /hardware/wiring.md for sensor hookup.
// ---------------------------------------------------------------------------

// --- Pins -------------------------------------------------------------------

// TCRT5000 phototransistor output (analog). A1 = GPIO3 on the XIAO ESP32-C3
// (ADC-capable). Wiring: collector to this pin with a 10k pull-up to 3V3,
// emitter to GND — more reflection pulls the reading LOW.
#define PIN_SENSOR A1

// --- Optical sensing tunables -------------------------------------------------
// Target (M0a, confirmed): the winder launcher's factory encoder disc —
// black/white two-segment, seen through the ~5x5mm top-face window, disc
// surface ~3mm below. Threshold PLACEHOLDERS until M0b measures the real
// white/black IR swing: use raw mode (send 'r' over serial) to stream ADC
// samples and pick thresholds from what you see.

// Software Schmitt trigger, 12-bit ADC counts (0–4095). A pulse registers
// when the reading falls below LOW (reflective feature in view); the
// detector re-arms when it rises back above HIGH. Gap between them is the
// hysteresis — keep it wider than the observed noise band.
#define SENSOR_THRESH_LOW  1600
#define SENSOR_THRESH_HIGH 2400

// Confirmed M0a: two-segment encoder disc = one dark/light cycle per
// encoder revolution. Used directly in the RPM math.
#define PULSES_PER_REV 1

// TODO: encoder-shaft-to-hook ratio, pending tooth count —
// hook_rpm = encoder_rpm * GEAR_RATIO. Applied in the RPM math (and thus
// the BLE launch record).
#define GEAR_RATIO 1.0f

// Glitch filter: reject pulse intervals shorter than this. Size it from
// PULSES_PER_REV and max plausible RPM: at 15,000 RPM (250 rev/s) and
// 8 pulses/rev, real intervals are ~500µs — so 200µs rejects noise without
// eating real pulses. Retune when M0a fixes the real pulses/rev.
#define MIN_PULSE_INTERVAL_US 200UL

// --- Launch detection -----------------------------------------------------------

#define SERIAL_BAUD 115200

// Launch is considered over after this long with no pulses (SPEC §4).
#define LAUNCH_END_TIMEOUT_MS 250UL

// Ignore "launches" with fewer pulses than this (a lone pulse gives no
// interval, so no RPM).
#define MIN_PULSES_PER_LAUNCH 2

// How often to push live RPM to the BLE training-mode stream while spinning
// (no-op in serial-only builds).
#define LIVE_RPM_PERIOD_MS 100UL

// --- Power (SPEC §4, builds with -DOPENRIP_ENABLE_SLEEP) ----------------------

// Wake/user button, wired to GND (internal pull-up). D2 = GPIO4 on the XIAO
// ESP32-C3; deep-sleep GPIO wake on the C3 only works on GPIO0–5.
#define PIN_BUTTON D2

// Deep sleep after this long with no pulses and no BLE client (SPEC: 5 min).
#define IDLE_SLEEP_TIMEOUT_MS (5UL * 60UL * 1000UL)
