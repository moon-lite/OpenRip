#pragma once

// ---------------------------------------------------------------------------
// OpenRip firmware configuration — pin assignments and tunables.
// Board: Seeed XIAO ESP32-C3. See /hardware/wiring.md for sensor hookup.
// ---------------------------------------------------------------------------

// --- Pins -------------------------------------------------------------------

// Hall sensor signal input. D1 = GPIO3 on the XIAO ESP32-C3.
// Firmware counts digital pulses: use a digital-output hall (e.g. A3144,
// open collector) or an SS49E fed through a comparator. A raw SS49E analog
// output will NOT trigger clean interrupts — see /hardware/wiring.md.
#define PIN_HALL D1

// INPUT_PULLUP suits an open-collector hall (A3144): idle high, magnet pulls
// low. Switch to INPUT if using an external pull-up or a push-pull comparator.
#define HALL_PIN_MODE INPUT_PULLUP

// Interrupt edge. FALLING pairs with open-collector + pull-up above.
#define HALL_TRIGGER_EDGE FALLING

// --- Tunables ----------------------------------------------------------------

#define SERIAL_BAUD 115200

// Magnets on the launcher chuck → hall pulses per chuck revolution (SPEC §4:
// one 3mm magnet, 1 pulse/rev).
#define PULSES_PER_REV 1

// Glitch filter: reject pulse intervals shorter than this (switch bounce,
// EMI). 500 µs floor still allows 120,000 RPM at 1 pulse/rev — far above any
// real launch (5–12k RPM).
#define MIN_PULSE_INTERVAL_US 500UL

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
