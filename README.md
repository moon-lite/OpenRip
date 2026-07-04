# OpenRip

Open-source launch telemetry for Beyblade X. A clip-on module for string
launchers — Seeed XIAO ESP32-C3 + hall sensor — that measures launch RPM
(and, in v0.2, launch angle via IMU) and reports over BLE to a Web Bluetooth
app. ~$20 in parts, a 3D printer, basic soldering. See [SPEC.md](SPEC.md)
for the full spec.

> **Trademark note:** "BattlePass" and "Beyblade" are Takara Tomy/Hasbro
> trademarks. This project is not affiliated with or endorsed by either, and
> uses those names only descriptively ("compatible with Beyblade X
> launchers").

## Milestones

| Milestone | Scope | Status |
|---|---|---|
| M0 | Sensor proof — breadboard hall + ESP32, RPM over serial | 🚧 Awaiting hardware validation |
| M1 | v0.1 device — BLE launch records, printed housing, battery | 🚧 Firmware done, housing/battery pending |
| M2 | Web Bluetooth app on GitHub Pages | 🚧 [App live](https://moon-lite.github.io/OpenRip/), needs on-device testing |
| M3 | Public v0.1 — build guide, ESP Web Tools flashing | Planned |
| M4 | v0.2 — IMU launch angle, calibration, smoothness | Planned |
| M5 | Custom PCB + kits (optional) | Maybe |

## Repo layout

```
/firmware   PlatformIO project (XIAO ESP32-C3, Arduino framework)
/hardware   BOM, wiring guide
/models     Housing CAD (from M1)
/app        Web Bluetooth app (from M2)
/docs       Build guide, calibration
SPEC.md     Full project spec — source of truth
```

## Building the firmware

```sh
cd firmware
pio run                 # build (serial-only M0)
pio run -t upload       # flash over USB-C
pio device monitor      # launch summaries at 115200 baud
```

**Browser-based flashing via ESP Web Tools arrives at M3** — the flash page
stub lives at `app/flash/`, but until a release ships a firmware binary,
flashing requires PlatformIO.

## License

Firmware and app: MIT (planned). Hardware and models: CERN-OHL-P (planned).
