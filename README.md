# OpenRip

**Open-source launch telemetry for Beyblade X.** A ~$20 module that mounts
on top of the winder launcher — touching nothing but the outside — and
measures your launch RPM, beaming it to a web app over Bluetooth. No app
store, no account, no proprietary anything.

Think of it as an open alternative to the official Beybattle Pass — build
it, hack it, own your data — plus measurements the official one doesn't
take.

> ⚠️ **Project status: early prototype.** The firmware and web app exist,
> and the sensing approach is confirmed (the launcher has a factory optical
> encoder we read through its window — zero modification). But the signal
> hasn't been proven on a real launcher yet and there's no printed housing.
> Nothing here is ready to build unless you enjoy breadboards. Watch/star
> the repo to catch the first buildable release.

## What it does

- **Peak and release RPM** from an IR sensor reading the winder launcher's
  **factory encoder disc** through its built-in window — the exact surface
  the official module reads. **Zero launcher modification** (targeting ±2%
  of the official numbers)
- **True launch detection**: a low-force switch rides the launcher's
  bey-presence plunger, so tracking arms when a bey locks in and the
  release moment is timestamped — winding and re-insertion can't fake a
  launch
- **Live RPM + launch history** in a [web app](https://moon-lite.github.io/OpenRip/)
  that connects straight from Chrome — history stays in your browser, exports
  to CSV
- **Coming in v0.2:** launch angle relative to the stadium floor, and a rip
  smoothness score, via an IMU — data the official module doesn't capture

## What's in the box

| | |
|---|---|
| Brains | Seeed XIAO ESP32-C3 (~$5) |
| Sensor | TCRT5000 IR reflective sensor over the encoder window (~$1.50) |
| Presence | Subminiature microswitch riding the bey-release plunger (~$2) |
| Power | 100mAh LiPo, charges over USB-C (~$4) |
| Body | 3D-printed shell mounting on the winder launcher's top face |

Full parts list in [`hardware/BOM.csv`](hardware/BOM.csv), wiring in
[`hardware/wiring.md`](hardware/wiring.md).

## Try the app

The web app is live at **[moon-lite.github.io/OpenRip](https://moon-lite.github.io/OpenRip/)** —
no hardware needed to poke around: hit **SIM LAUNCH** to see it work.
Real connections need Chrome or Edge (Web Bluetooth doesn't exist on iOS).

## Roadmap

1. ✅ Firmware + web app written
2. ✅ **M0a: launcher characterized** — it has a factory optical encoder on
   the hook shaft, readable through a top-face window, plus a bey-presence
   plunger we piggyback for launch detection
3. 🔜 **M0b: signal proof** — clean pulse train off a real launcher
   ← we are here (waiting on parts)
4. Printed housing + battery → first real device
5. Public v0.1: build guide, video, flash-from-browser
6. v0.2: IMU for launch angle + smoothness

Details live in [SPEC.md](SPEC.md); build instructions grow in
[`docs/build-guide.md`](docs/build-guide.md) as each stage lands.

## For developers

```sh
cd firmware
pio run                 # build (serial-only)
pio run -t upload       # flash over USB-C
pio device monitor      # launch summaries at 115200 baud
```

Flashing from the browser (ESP Web Tools) ships with the first release —
the page is already stubbed at [`app/flash/`](app/flash/).

## Legal

BEYBLADE, BEYBLADE X, and BEYBATTLE PASS are trademarks of Tomy Company,
Ltd. and/or its licensee Hasbro, Inc. This project is not affiliated with,
sponsored by, or endorsed by either company; the names are used only to
describe compatibility ("works with Beyblade X winder launchers"). All
other trademarks are the property of their respective owners.

Licenses: firmware and app are MIT (see [LICENSE](LICENSE)); hardware and
models are CERN-OHL-P v2 (see [hardware/LICENSE](hardware/LICENSE) and
[models/LICENSE](models/LICENSE)).
