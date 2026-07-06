# Sensor Characterization

## M0a — Mount characterization ✅ FULLY COMPLETE (2026-07, incl. gearing teardown)

**Finding: the winder launcher has a factory optical encoder.** Sensing is
confirmed viable with zero modification — we read the same surface the
official device reads.

- **What's there:** a black/white **two-segment encoder disc**, visible
  through a **~5×5mm clear window** on the launcher's **top face**.
- **Working distance:** disc surface sits **~3mm below the window** — right
  at the TCRT5000's ~2.5mm sensitivity peak with the sensor face flush to
  the window.
- **Pulses per revolution:** two segments → **1 dark/light cycle per
  encoder revolution** (`PULSES_PER_REV 1` in config.h).
- **Gearing (confirmed by teardown):** the encoder disc is mounted
  directly on the **hook shaft** (16T gear), downstream of the gear
  train — the window reads **hook RPM directly**. `GEAR_RATIO = 1.0`
  confirmed; the constant remains in config.h for potential future
  launcher variants.
- **Remaining signal risk (low):** IR contrast between the *translucent*
  white segment and the black segment at the TCRT5000's wavelength. The
  disc was designed for the official sensor's optics, not ours — M0b
  measures the actual swing.
- **Mechanical note:** the TCRT5000 package is wider than the window; the
  domes partially overhang the opening (see hardware/wiring.md). ITR8307
  is the documented fallback if the swing is poor.

## M0b — Signal proof (next)

1. Breadboard per `hardware/wiring.md`; hold the sensor flush over the
   window, centered (tape/jig is fine at this stage).
2. `pio device monitor`, send `r` → raw mode streams `micros,adc` lines.
3. **Hand-crank the winder slowly.** Verify a clean two-level signal:
   white segment = low ADC (more reflection), black = high. Capture a few
   revolutions and save the dump under `docs/captures/`.
4. From the capture, set in `firmware/include/config.h`:
   - `SENSOR_THRESH_LOW` just above the white level
   - `SENSOR_THRESH_HIGH` just below the black level
   - (noise band must fit inside the gap — widen it if crossings chatter)
5. Rebuild, hand-crank again in normal mode: pulse count should match
   crank revolutions × gearing exactly. Chase any mismatch before step 6.
6. **Live launches.** The encoder sits on the hook shaft, so readings are
   hook RPM directly — sanity range 5,000–12,000.

## Findings log

<!-- M0b: raw captures, measured white/black ADC levels, chosen thresholds,
     gear tooth counts when opened/counted, launch pulse counts. -->
