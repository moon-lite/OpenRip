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

## M0b — Signal proof ✅ complete (2026-07)

1. Breadboard per `hardware/wiring.md`: sensor flush over the window,
   centered, AND the plunger microswitch lever positioned over the spring
   plunger next to the window (tape/jig is fine at this stage).
2. **Verify the plunger state machine first** (no spinning needed). Watch
   the serial monitor's `# state:` lines:
   - lock a bey in → plunger rises → `# state: ARMED`
   - pull the bey back out without launching → `# state: IDLE`
   - repeat a few times; transitions must be clean single edges (no
     chatter — if you see bouncing, raise `PLUNGER_DEBOUNCE_MS`).
   - confirm bey seating feels identical with the switch in place — the
     lever must not fight the plunger.
3. `pio device monitor`, send `r` → raw mode streams `micros,adc` lines.
4. **Hand-crank the winder slowly** (bey locked in, so pulses register).
   Verify a clean two-level signal: white segment = low ADC (more
   reflection), black = high. Capture a few revolutions and save the dump
   under `docs/captures/`.
5. From the capture, set in `firmware/include/config.h`:
   - `SENSOR_THRESH_LOW` just above the white level
   - `SENSOR_THRESH_HIGH` just below the black level
   - (noise band must fit inside the gap — widen it if crossings chatter)
6. Rebuild, hand-crank again in normal mode: pulse count should match
   crank revolutions exactly. Chase any mismatch before step 7.
7. **Live launches.** Watch for `# state: RIPPING` then a launch summary
   whose `end trigger` says **release edge** — a "pulse timeout (fallback)"
   trigger on a real launch means the switch missed the release; fix the
   lever geometry. Readings are hook RPM directly — sanity range
   5,000–12,000. Both peak and release RPM should report, with release a
   bit below peak.

## Findings log

- **Hand-crank capture (breadboard, IR sensor module with onboard LM393
  comparator — AO output wired to XIAO D1/A1, no discrete resistors
  needed):**
  - Quiet baseline: ~190–230 ADC counts, low noise (±20ish).
  - Rotation peaks: 600–2400+ depending on hand-crank speed; weakest
    observed peak ~600.
  - Clean, monotonic edges, spaced ~300–380ms apart during a slow steady
    hand-crank (~3 rev/s).
  - Chosen thresholds: `SENSOR_THRESH_LOW 400`, `SENSOR_THRESH_HIGH 550` —
    clears the baseline noise floor and still reliably re-arms on the
    weakest observed peaks. (Prior defaults of 1600/2400 would have missed
    re-arming on weak cranks entirely.)
- **Plunger microswitch:** wired (COM → D3, NO → GND) and validated —
  `IDLE`/`ARMED`/`RIPPING` states transition cleanly. Pin ID note: on this
  switch only **C** (=COM) and **NC** are silkscreened; the unlabeled
  third pin is **NO**.
- **First real launch reports** (bench test, no housing yet — switch
  manually held/released by hand instead of tracking the real plunger):
  - Peak RPM 6746 and 6654 — both inside the expected 5,000–12,000
    hook-RPM range. Confirms the sensor and `GEAR_RATIO` math end-to-end.
  - All three test launches ended via `pulse timeout (fallback)` rather
    than `release edge` — expected here, not a defect: release RPM was far
    below peak RPM in each (bey spinning down while still seated, not
    physically ejected). Getting a real release-edge trigger needs the
    switch tracking the actual plunger, which is genuinely fiddly at
    breadboard scale given only ~1mm of travel — this is exactly what M1's
    adjustable carriage (build-guide.md) is for.
- **Go/no-go: PASSED.** Clean two-level pulse train confirmed off a real
  launcher, with plausible RPM numbers. M0b complete; M1 (printed housing,
  battery, BLE records) is next.
