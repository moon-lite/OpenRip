# Wiring — Hall Sensor → XIAO ESP32-C3

M0 breadboard hookup. The firmware counts **digital pulses** on `PIN_HALL`
(D1 = GPIO3, configured in `firmware/include/config.h`), one pulse per chuck
revolution from a 3mm neodymium magnet on the launcher chuck.

Two sensor options — pick one:

## Option A — A3144 (digital, open-collector) · simplest for M0

| A3144 pin | XIAO ESP32-C3 |
|---|---|
| 1 (VCC) | **5V** pin (USB VBUS) |
| 2 (GND) | GND |
| 3 (OUT) | D1 (GPIO3) |

- The A3144 is spec'd for a 4.5–24V supply, so power it from the XIAO's 5V
  pin. Many clones run at 3.3V but that's out of spec — don't rely on it.
- The output is **open-collector**, so it never drives the pin high — it only
  pulls to GND. With the firmware's internal pull-up (`INPUT_PULLUP`) the
  3.3V GPIO is safe even with a 5V supply. No external resistor needed;
  add a 10kΩ external pull-up to **3V3** if you see noise on long leads.
- Idle = high, magnet pass = low → firmware triggers on `FALLING` (default).
- The A3144 is unipolar: it switches on one magnet pole only. If you get no
  pulses, flip the magnet.

## Option B — SS49E (analog, per the BOM) · needs a comparator

| SS49E pin | XIAO ESP32-C3 |
|---|---|
| 1 (VCC) | 3V3 |
| 2 (GND) | GND |
| 3 (OUT) | → comparator → D1 (GPIO3) |

- The SS49E runs happily at 3.3V (2.7–6.5V range) and outputs an **analog**
  voltage centered at ~VCC/2 that swings with field strength.
- The M0 firmware expects clean digital edges on a GPIO interrupt — a raw
  SS49E output will not trigger reliably. Add a comparator (e.g. LM393 with
  a trimpot threshold, output pulled up to 3V3) between OUT and D1.
- Alternative (not in M0 firmware): sample the SS49E with the ADC and detect
  peaks in software. Deferred — interrupt counting is simpler and faster.
- Upside of the SS49E: analog field strength readout makes it useful for
  validating the sensor–magnet air gap (~2–4mm), the primary technical risk
  called out in SPEC §3.

## Notes

- Signal pin, pin mode, and trigger edge are set in
  `firmware/include/config.h` (`PIN_HALL`, `HALL_PIN_MODE`,
  `HALL_TRIGGER_EDGE`).
- Battery (401030 LiPo) and slide switch wiring lands with M1; for M0 run
  from USB-C.
- Validate sensor placement on a breadboard jig before any CAD (SPEC §3).
