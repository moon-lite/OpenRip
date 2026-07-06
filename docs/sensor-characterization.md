# Sensor Characterization

## M0a — Mount characterization ✅ COMPLETE (2026-07)

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
- **Open item:** encoder-shaft-to-hook gear ratio (tooth count not yet
  counted) — `GEAR_RATIO` in config.h stays 1.0 until it is;
  hook_rpm = encoder_rpm × GEAR_RATIO.
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
6. **Live launches.** Sanity range for hook RPM is 5,000–12,000 once
   `GEAR_RATIO` is real; with GEAR_RATIO=1.0 you're reading raw encoder
   RPM — record it and note the ratio is pending.

## Findings log

<!-- M0b: raw captures, measured white/black ADC levels, chosen thresholds,
     gear tooth counts when opened/counted, launch pulse counts. -->
