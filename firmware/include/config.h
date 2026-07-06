#pragma once

// ---------------------------------------------------------------------------
// OpenRip firmware configuration — pin assignments and tunables.
// Board: Seeed XIAO ESP32-C3. See /hardware/wiring.md for sensor hookup.
// ---------------------------------------------------------------------------

// --- Pins -------------------------------------------------------------------

// QRE1113 phototransistor output (analog). A1 = GPIO3 on the XIAO ESP32-C3
// (ADC-capable). Wiring: collector to this pin with a 10k pull-up to 3V3,
// emitter to GND — more reflection pulls the reading LOW.
#define PIN_SENSOR A1

// --- Optical sensing tunables -------------------------------------------------
// PLACEHOLDERS until M0a characterizes the real signal off a stock launcher
// spool through the mount opening. Use raw mode (send 'r' over serial) to
// stream ADC samples and pick real thresholds from the observed swing.

// Software Schmitt trigger, 12-bit ADC counts (0–4095). A pulse registers
// when the reading falls below LOW (reflective feature in view); the
// detector re-arms when it rises back above HIGH. Gap between them is the
// hysteresis — keep it wider than the observed noise band.
#define SENSOR_THRESH_LOW  1600
#define SENSOR_THRESH_HIGH 2400

// Optical features on the spool visible per revolution (spokes, ribs, color
// changes — or the fallback sticker's marks). TBD from M0a; default 1.
// Used directly in the RPM math.
#define PULSES_PER_REV 1

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
