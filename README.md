# OpenRip

**Open-source launch telemetry for Beyblade X.** A ~$20 clip-on module for
string launchers that measures your launch RPM — and eventually launch angle
— and beams it to a web app over Bluetooth. No app store, no account, no
proprietary anything.

Think of it as a BattlePass you can build, hack, and actually own the data
from — plus measurements the official one doesn't take.

> ⚠️ **Project status: early prototype.** The firmware and web app exist, but
> the sensor hasn't been validated on a real launcher yet and there's no
> printed housing. Nothing here is ready to build unless you enjoy
> breadboards. Watch/star the repo if you want to catch the first buildable
> release.

## What it does

- **Peak launch RPM** from an IR reflective sensor reading the launcher's
  spool through the built-in BattlePass mount opening — **zero launcher
  modification**, same sensing approach as the official module (targeting
  ±2% of its numbers)
- **Live RPM + launch history** in a [web app](https://moon-lite.github.io/OpenRip/)
  that connects straight from Chrome — history stays in your browser, exports
  to CSV
- **Coming in v0.2:** launch angle relative to the stadium floor, and a rip
  smoothness score, via an IMU — data the official module doesn't capture

## What's in the box

| | |
|---|---|
| Brains | Seeed XIAO ESP32-C3 (~$5) |
| Sensor | QRE1113 IR reflective sensor aimed through the mount opening (~$1) |
| Power | 100mAh LiPo, charges over USB-C (~$4) |
| Body | 3D-printed clip-on shell for the string launcher grip rail |

Full parts list in [`hardware/BOM.csv`](hardware/BOM.csv), wiring in
[`hardware/wiring.md`](hardware/wiring.md).

## Try the app

The web app is live at **[moon-lite.github.io/OpenRip](https://moon-lite.github.io/OpenRip/)** —
no hardware needed to poke around: hit **SIM LAUNCH** to see it work.
Real connections need Chrome or Edge (Web Bluetooth doesn't exist on iOS).

## Roadmap

1. ✅ Firmware + web app written
2. 🔜 **M0a: characterize the mount opening** — what the sensor can see of
   the rotating spool, working distance, features per rev ← we are here
3. **M0b: sensor proof** — clean optical pulse train off a real launcher
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

"Beyblade" and "BattlePass" are trademarks of Takara Tomy / Hasbro. This
project is not affiliated with, endorsed by, or connected to either company;
the names are used only descriptively ("compatible with Beyblade X
launchers").

Licenses (planned): MIT for firmware and app, CERN-OHL-P for hardware and
models.
