# OpenRip Build Guide

> Skeleton — sections fill in as each milestone lands. SPEC.md §8 is the
> source of truth for milestone scope.

## M0a — Mount characterization

<!-- Photograph the BattlePass mount opening, identify the rotating surface
     and its optical features, measure working distance. Procedure:
     /docs/sensor-characterization.md. Gates M0b. -->

## M0b — Sensor proof (breadboard)

<!-- Parts, breadboard wiring (see /hardware/wiring.md), raw-mode threshold
     tuning, flashing over USB, reading launch summaries over serial.
     Go/no-go gate: clean pulse train off a real launcher. -->

## M1 — v0.1 device

<!-- Soldering, battery + switch, printed housing assembly (see /models),
     BLE launch records. -->

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
