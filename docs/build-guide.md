# OpenRip Build Guide

> Skeleton — sections fill in as each milestone lands. SPEC.md §8 is the
> source of truth for milestone scope.

## M0a — Mount characterization

<!-- Photograph the launcher's top-face encoder window, identify the rotating surface
     and its optical features, measure working distance. Procedure:
     /docs/sensor-characterization.md. Gates M0b. -->

## M0b — Sensor proof (breadboard) ✅ complete

Parts and wiring: `hardware/wiring.md`. Full procedure and measured
findings: `docs/sensor-characterization.md`.

Short version: flash the `m0-serial` env, `pio device monitor`, send `r`
for raw ADC mode, hand-crank the winder to capture real white/black
levels, set `SENSOR_THRESH_LOW`/`HIGH` in `firmware/include/config.h`
from what you see, reflash, and confirm the pulse count matches crank
revolutions. Wire the plunger microswitch (COM → D3, NO → GND) to unlock
the presence-gated launch report — that gives an exact pulse/RPM readout
instead of hand-counting raw ADC lines.

Go/no-go gate — **passed**: clean two-level pulse train off a real
launcher, launch RPM readings landing in the expected 5,000–12,000 range.

## M1 — v0.1 device

<!-- Soldering, battery + switch, printed housing assembly (see /models),
     BLE launch records. -->

### Switch carriage calibration (once per build)

The plunger only rises ~1mm above flush, so the presence switch sits on an
adjustable carriage (slot + M2 screw) instead of a fixed pocket.

1. Mount the housing on the launcher, carriage screw loose.
2. With a bey **seated**: slide the carriage toward the plunger until the
   switch clicks (serial shows `# state: ARMED`).
3. Remove the bey: the switch must release (`# state: IDLE`). If it stays
   clicked, back the carriage off a hair and repeat.
4. Tighten the M2 screw and re-check both states, plus seating feel — the
   lever must not resist the bey clicking in.

## M2 — App

<!-- Connecting from the Web Bluetooth app, launch history, CSV export. -->

## M3 — Public v0.1

<!-- Flashing from the browser with ESP Web Tools; companion video.
     Flash page stub: /app/flash/ (index.html + manifest.json). Each release
     needs a single merged binary at offset 0, built from the pio output:
       esptool.py --chip esp32c3 merge_bin -o openrip-merged.bin \
         0x0    firmware/.pio/build/m0-serial/bootloader.bin \
         0x8000 firmware/.pio/build/m0-serial/partitions.bin \
         0x10000 firmware/.pio/build/m0-serial/firmware.bin
     Attach it to the GitHub Release and point manifest.json at it. -->


## M4 — v0.2 IMU

<!-- IMU install, launch angle, calibration (see calibration.md),
     smoothness scoring. -->

## M5 — Custom PCB (optional)

<!-- Small-batch PCB path. -->
