# OpenRip — Open Source Launch Telemetry for Beyblade X

**Status:** Draft v0.1 · **License targets:** MIT (firmware/app), CERN-OHL-P (hardware/models)

An open-source, clip-on launcher module that measures launch RPM and launch angle for Beyblade X, reporting over BLE. Functional alternative to the official BattlePass with data it doesn't capture (IMU) and an open protocol.

> Naming note: "BattlePass" and "Beyblade" are Takara Tomy/Hasbro trademarks. Project name and repo must not use them except descriptively ("compatible with Beyblade X launchers").

---

## 1. Goals

- **Zero launcher modification (hard requirement):** the device attaches externally (top-face mount over the factory encoder window) — nothing glued, pressed on, or altered. This is what keeps tournament use on the table.
- Measure peak launch RPM with accuracy comparable to the official BattlePass (±2% target)
- Measure launch angle relative to a calibrated stadium plane
- Capture rip motion profile (accel curve, smoothness/consistency score)
- Report over BLE to a Web Bluetooth app — no app store, no account
- Fully reproducible from the repo: ~$20 in parts, a 3D printer, basic soldering
- Non-devs can flash firmware from a browser (ESP Web Tools)

## 2. Non-Goals (v0.x)

- String launcher support — v0.1 targets the winder launcher only (it has the factory optical encoder + window the sensor reads; string launcher sensing is a separate problem)
- Bey-side sensing (MEMS gyros saturate ~333 RPM; bey spins 5–12k)
- Position tracking / dead reckoning (IMU drift makes it useless)
- Battle outcome simulation — contact physics is out of scope
- Official app compatibility (protocol mimicry is a maybe-later, see §9)
- iOS support in v0.1 (Web Bluetooth is dead on iOS Safari)

## 3. Hardware

### v0.1 — Spin only (prototype)
| Part | Component | Est. cost |
|---|---|---|
| MCU | Seeed XIAO ESP32-C3 | $5 |
| Spin sensor | TCRT5000 IR reflective sensor, reading the winder launcher's **factory optical encoder** — a black/white two-segment disc visible through a ~5×5mm clear window on the top face, disc surface ~3mm below the window (this is what the official device reads; launcher stays stock). Peak sensitivity ~2.5mm matches the recess. Fallback: ITR8307 if the TCRT5000's signal swing is poor. | $1 |
| Bey presence | Subminiature low-force lever microswitch (SPDT, <0.5N actuation, Omron SS-01GL class) riding the launcher's spring plunger next to the encoder window — plunger sits below flush when empty, above flush with a bey locked, drops on release. Gates launch detection and timestamps release. | $1.50 |
| Battery | 401030 LiPo 100mAh (XIAO onboard charging) | $4 |
| Switch | SPDT slide | $0.50 |
| Housing | PLA, top-face mount over the encoder window (winder launcher) | ~$0.30 |

### v0.2 — Adds IMU
| Part | Component | Est. cost |
|---|---|---|
| IMU | LSM6DSO 6-axis (I2C) on launcher body | $5–8 |

**Signal-quality risk is now LOW** (M0a complete, 2026-07): the launcher ships with a purpose-built encoder disc — we're reading a surface designed to be read. The remaining unknown is IR contrast between the **translucent white** and black segments at the TCRT5000's wavelength (the disc was tuned for the official sensor, not ours); M0b measures it. Sticker fallbacks are obsolete.

### Small-batch path (later)
Custom PCB (ESP32-C3 module + bare LSM6DSO + TCRT5000), JLCPCB assembly: ~$8–12/unit @ qty 50.

## 4. Firmware

- **Stack:** PlatformIO, Arduino framework on ESP32-C3
- **RPM:** TCRT5000 reflectivity edges off the factory encoder disc → ADC threshold with hysteresis → interval timing. Two-segment disc = 1 pulse/encoder-rev; the disc sits on the hook shaft, so the window reads hook RPM directly (GEAR_RATIO = 1.0, kept configurable). Serial raw mode streams ADC samples for signal characterization.
- **Launch detection (presence-gated state machine):** IDLE → ARMED (plunger high = bey locked) → RIPPING (pulses while armed) → RELEASE (plunger falling edge). The release edge is timestamped; the record carries both **peak RPM** (min interval in the window) and **release RPM** (last pulse interval before the edge). Pulses are ignored while the plunger is low, which rejects winding/re-insertion as false launches. The 250ms pulse timeout survives only as a fallback when the release edge is missed; the switch input is debounced ~5ms. This matches the presumed official BattlePass behavior (presence-gated tracking) and resolves the launch-detection ambiguity of a pure timeout.
- **IMU (v0.2):** LSM6DSO @ 416Hz over I2C, Madgwick fusion for orientation. Log accel/orientation ring buffer during rip; snapshot orientation at final encoder pulse = release angle.
- **Calibration:** button-triggered 2s rest on stadium floor (or rim jig) → zero orientation to stadium plane. Store in NVS.
- **BLE:** GATT server, custom service. Notify characteristic pushes a launch record post-launch; second characteristic for live RPM stream (training mode).
- **Power:** deep sleep after 5 min idle, wake on motion (IMU interrupt) or button.

### Launch record (v1 schema)
```
{
  seq: uint16,
  peak_rpm: uint16,
  release_rpm: uint16,       // from last pulse interval before the plunger release edge
  release_pitch_deg: int8,   // v0.2
  release_roll_deg: int8,    // v0.2
  rip_duration_ms: uint16,
  smoothness: uint8,         // 0-100, v0.2
  battery_pct: uint8
}
```

## 5. App

- **v0.1:** single-page Web Bluetooth app (vanilla JS or lightweight framework). Connect, live RPM, launch history in localStorage, CSV export.
- **Hosting:** GitHub Pages from the same repo.
- **v0.2+:** angle display, consistency graphs, session stats.

## 6. Housing

- Modeled in Fusion; publish **STEP + STL + source (.f3d)**
- Mounts on the **top face of the winder launcher**, over the factory encoder window — registers against the top-face geometry (not the grip rail)
- Holds the TCRT5000 centered over the ~5×5mm window at ~0mm standoff (sensor face flush to the window; the disc sits ~3mm below, at the sensor's sweet spot)
- Positions the bey-presence microswitch lever over the spring plunger adjacent to the window — actuated by the plunger's above-flush travel **without adding force that prevents bey seating** (low-force lever, free pivot)
- The microswitch mounts on an **adjustable carriage** (slot + M2 screw), not a fixed pocket: the plunger only rises ~1mm above flush, which is inside FDM print-tolerance stack-up. Builders calibrate the position once — slide until the switch **clicks with a bey seated and releases with it out** — then lock the screw.
- Two-part shell: sensor carrier + electronics bay
- Print settings documented; no supports as a design goal

## 7. Repo Layout

```
/firmware      PlatformIO project
/hardware      BOM.csv, wiring diagram, (later: KiCad)
/models        STEP, STL, Fusion source
/app           Web Bluetooth app (deployed via Pages)
/docs          Build guide, sensor placement photos, calibration
SPEC.md        this document
```

- GitHub Releases: firmware .bin + ESP Web Tools flash page per release
- Build guide gets a companion video (3 min, embedded in README)

## 8. Milestones

- **M0a — Mount characterization: ✅ complete (2026-07).** Winder launcher has a factory optical encoder — two-segment black/white disc through a ~5×5mm top-face window, disc ~3mm down. Findings: docs/sensor-characterization.md.
- **M0b — Sensor proof:** breadboard TCRT5000 + ESP32, clean two-level signal off the encoder disc (hand-crank first, then real launches), serial output. *Go/no-go gate for everything else.*
- **M1 — v0.1 device:** BLE launch records, printed housing, battery power
- **M2 — App:** Web Bluetooth page live on Pages, history + export
- **M3 — Public v0.1:** repo cleanup, build guide + video, ESP Web Tools flashing. Post to WBO + r/BeybladeX
- **M4 — v0.2 IMU:** angle, calibration, smoothness scoring
- **M5 (optional):** custom PCB + Tindie kits if community demand shows

## 9. Open Questions

1. Verify official BattlePass mount geometry vs. designing our own rail clamp (affects M1 CAD)
2. SP↔RPM relationship — worth matching official app numbers for community comparability?
3. Mimic the reverse-engineered official BLE protocol (WBO thread) so community apps work, or stay clean with our own? Leaning own protocol + documented schema.
4. ~~Magnet on chuck: glue-on acceptable, or design a press-fit printed chuck sleeve so the launcher stays unmodified?~~ Superseded: any launcher modification is out (see Goals). Optical sensing through the mount opening replaces the magnet entirely.
5. ~~Pulses-per-rev off stock geometry~~ Resolved at M0a: two-segment encoder disc = 1 pulse per encoder rev. ~~Encoder-to-hook gear ratio~~ Resolved by teardown: the disc is mounted directly on the hook shaft (16T gear), downstream of the gear train — the window reads hook RPM directly (GEAR_RATIO = 1.0).
6. Tournament legality is TO-discretion even at zero modification — most rulesets allow *official* accessories, not arbitrary attachments. Frame the device as training/casual-legal, tournament-pending; zero modification keeps the door open, it doesn't guarantee entry.
