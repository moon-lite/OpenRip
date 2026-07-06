# Wiring — QRE1113 Optical Sensor → XIAO ESP32-C3

M0 breadboard hookup. The firmware samples the QRE1113's phototransistor on
an **ADC pin** (`PIN_SENSOR` = A1 = GPIO3, set in `firmware/include/config.h`)
and converts reflectivity changes into pulses with a software Schmitt
trigger. The sensor aims at the launcher's exposed rotating spool through
the BattlePass mount opening — **the launcher is never modified**.

## Bare QRE1113 (4 pins, through-hole)

The package has two sides: an IR LED (emitter) and a phototransistor
(detector). Check the datasheet corner marking for pin 1.

| QRE1113 pin | Via | XIAO ESP32-C3 |
|---|---|---|
| LED anode (1) | 100Ω resistor | 3V3 |
| LED cathode (2) | — | GND |
| Phototransistor collector (3) | 10kΩ pull-up to 3V3, tap to pin | A1 (GPIO3) |
| Phototransistor emitter (4) | — | GND |

- **Emitter resistor:** (3.3V − ~1.2V Vf) / 100Ω ≈ 21mA — inside the LED's
  rating. Raise to 220Ω to save power at reduced range once the working
  distance is known.
- **Signal sense:** more IR reflected → more phototransistor current → the
  pulled-up collector reads **lower**. A bright/reflective feature passing
  the sensor is a dip in the ADC reading; the firmware registers the pulse
  on the falling crossing (`SENSOR_THRESH_LOW`) and re-arms on the rising
  one (`SENSOR_THRESH_HIGH`).
- **Range:** the QRE1113 is happiest at ~1–3mm from the target. M0a
  measures what the mount opening actually gives us.

## SparkFun analog breakout (easier for breadboarding)

The [analog breakout](https://www.sparkfun.com/sparkfun-line-sensor-breakout-qre1113-analog.html)
has both resistors onboard: wire VCC → 3V3, GND → GND, OUT → A1. Same
signal sense (reflection = low). Buy the **analog** version, not the
digital one (the digital board's capacitor-discharge trick needs different
firmware).

## Characterizing the signal (M0a/M0b)

1. Flash and open the monitor: `pio run -t upload && pio device monitor`
2. Send `r` to toggle **raw mode** — the firmware streams `micros,adc`
   lines at max serial rate.
3. Hand-spin the spool slowly and watch the swing. Pick thresholds from
   what you see (defaults are placeholders): set `SENSOR_THRESH_LOW` just
   below the dips, `SENSOR_THRESH_HIGH` just above the bright-level noise
   band, and count the dips per revolution → `PULSES_PER_REV`.
4. Full procedure: `docs/sensor-characterization.md`.

## Notes

- All sensing tunables live in `firmware/include/config.h`
  (`PIN_SENSOR`, `SENSOR_THRESH_LOW/HIGH`, `PULSES_PER_REV`,
  `MIN_PULSE_INTERVAL_US`).
- Ambient IR (sunlight, incandescent) shifts the baseline — characterize
  indoors, and expect the housing to shroud the sensor at M1.
- Battery (401030 LiPo) and slide switch wiring lands with M1; for M0 run
  from USB-C.
