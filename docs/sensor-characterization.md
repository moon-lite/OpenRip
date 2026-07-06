# Sensor Characterization — M0a Procedure

> Stub — fill in with findings (photos, measurements, captures) as M0a runs.
> M0a gates M0b: no breadboard pulse-counting until we know what the sensor
> is actually looking at.

## Goal

Determine whether the launcher's stock spool, viewed through the BattlePass
mount opening, gives the QRE1113 a usable reflectivity signal — and if so,
how many pulses per revolution.

## 1. Photograph the mount opening

- Macro shots of the BattlePass mount rail area on the string launcher:
  straight-on and oblique.
- Identify what is visible through the opening and **what actually rotates**
  during a launch (spool? gear face? something else?).

## 2. Identify optical features

- On the rotating surface: spokes, ribs, holes, molding marks, color
  changes — anything that changes reflectivity as it passes.
- Count candidate features per revolution → the expected `PULSES_PER_REV`.
- If the surface is featureless/uniform: note it, and prepare the fallback
  (removable high-contrast sticker with 1–4 marks).

## 3. Measure working distance

- Distance from the mount-rail plane (where the device will sit) to the
  rotating surface. The QRE1113 wants ~1–3mm to the target.
- Note any parallax/angle constraints the opening imposes.

## 4. Capture the signal (raw mode)

- Breadboard per `hardware/wiring.md`, sensor held at the measured distance
  (tape/jig — rigid enough for a slow spin).
- `pio device monitor`, send `r` → streams `micros,adc` lines.
- **Slow hand-spin first**: rotate the spool by hand and capture several
  revolutions. Save the dump into this doc's `captures/` folder.
- Read off: baseline level, dip depth per feature, noise band. Set
  `SENSOR_THRESH_LOW/HIGH` (config.h) around what you see; confirm
  `PULSES_PER_REV`.
- Only then do real launches (M0b) and compare pulse counts against the
  hand-spin ground truth.

## Findings

<!-- Record everything here: photos, distances, pulse counts, chosen
     thresholds, whether the sticker fallback was needed. -->
