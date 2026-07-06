#pragma once

#include <stdint.h>

// Launch record, v1 schema — SPEC.md §4. This packed struct is the wire
// format pushed over the BLE notify characteristic (M1+); M0 prints the same
// fields over serial. Do not reorder fields without bumping the schema.
// (release_rpm added pre-release, 2026-07 — still v1; nothing had shipped.)
typedef struct __attribute__((packed)) {
    uint16_t seq;
    uint16_t peak_rpm;
    uint16_t release_rpm;        // last pulse interval before the plunger release edge
    int8_t   release_pitch_deg;  // v0.2 (IMU) — 0 until then
    int8_t   release_roll_deg;   // v0.2 (IMU) — 0 until then
    uint16_t rip_duration_ms;
    uint8_t  smoothness;         // 0–100, v0.2 — 0 until then
    uint8_t  battery_pct;
} launch_record_t;

static_assert(sizeof(launch_record_t) == 12, "launch_record_t must match the v1 wire format");
