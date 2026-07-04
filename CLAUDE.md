# OpenRip

Open-source launch telemetry for Beyblade X launchers: XIAO ESP32-C3 + hall
sensor measuring launch RPM, reported over serial (M0) and later BLE.

## Commands (run from /firmware)

- `pio run` — build (default env is serial-only M0)
- `pio run -t upload` — flash over USB
- `pio device monitor` — serial output at 115200

## Conventions

- Pin assignments and tunables live in `firmware/include/config.h`.
- **SPEC.md is the source of truth** for schemas (launch record), milestones,
  and scope. Don't change wire formats or milestone scope without it.
