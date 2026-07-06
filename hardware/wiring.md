# Wiring — TCRT5000 Optical Sensor → XIAO ESP32-C3

M0b breadboard hookup. The firmware samples the TCRT5000's phototransistor
on an **ADC pin** (`PIN_SENSOR` = A1 = GPIO3, set in
`firmware/include/config.h`) and converts reflectivity changes into pulses
with a software Schmitt trigger.

**Target (confirmed at M0a):** the winder launcher's factory optical
encoder — a black/white two-segment disc visible through a ~5×5mm clear
window on the launcher's top face, disc surface ~3mm below the window. The
launcher is never modified; the sensor just looks through the window the
official device uses.

## TCRT5000 hookup

The package has two domes: an IR LED (blue-tinted) and a phototransistor
(black). Datasheet: Vishay TCRT5000.

| TCRT5000 pin | Via | XIAO ESP32-C3 |
|---|---|---|
| LED anode (A) | 100Ω resistor | 3V3 |
| LED cathode (K) | — | GND |
| Phototransistor collector (C) | 10kΩ pull-up to 3V3, tap to pin | A1 (GPIO3) |
| Phototransistor emitter (E) | — | GND |

- **Emitter current:** (3.3V − ~1.25V Vf) / 100Ω ≈ 20mA — comfortable for
  the TCRT5000's LED.
- **Signal sense:** more IR reflected (white segment in view) → more
  phototransistor current → the pulled-up collector reads **lower**. The
  firmware registers a pulse on the falling crossing (`SENSOR_THRESH_LOW`)
  and re-arms on the rising one (`SENSOR_THRESH_HIGH`).
- **Distance:** peak sensitivity is ~2.5mm — a near-perfect match for the
  disc sitting ~3mm below the window with the sensor face flush against it.

### Package fit warning

The TCRT5000 body (~10.2 × 5.8mm) is **wider than the 5×5mm window** — the
emitter/detector domes partially overhang the window edges. On the
breadboard, center the package over the window as best you can and expect
some signal loss from the overhang. If the white/black swing in raw mode is
too small to threshold cleanly, the documented fallback is the
**ITR8307** (compact SMD, fits inside the window footprint) — same wiring
topology, same firmware.

## Characterizing the signal (M0b)

1. Flash and open the monitor: `pio run -t upload && pio device monitor`
2. Send `r` to toggle **raw mode** — the firmware streams `micros,adc`
   lines at max serial rate.
3. **Hand-crank the winder slowly** and watch the two-level signal: white
   segment = low readings, black = high. Set `SENSOR_THRESH_LOW` just above
   the white level, `SENSOR_THRESH_HIGH` just below the black level, with
   the noise band inside the gap.
4. Full procedure and M0a findings: `docs/sensor-characterization.md`.

## Notes

- All sensing tunables live in `firmware/include/config.h` (`PIN_SENSOR`,
  `SENSOR_THRESH_LOW/HIGH`, `PULSES_PER_REV`, `GEAR_RATIO`,
  `MIN_PULSE_INTERVAL_US`).
- The encoder disc gives 1 pulse/rev of the **encoder shaft** — hook RPM is
  encoder RPM × `GEAR_RATIO` (tooth count TBD; 1.0 until then).
- Ambient IR (sunlight, incandescent) shifts the baseline — characterize
  indoors; the M1 housing shrouds the sensor.
- Battery (401030 LiPo) and slide switch wiring lands with M1; for M0 run
  from USB-C.
