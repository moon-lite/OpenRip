# OpenRip — Open Source Launch Telemetry for Beyblade X

**Status:** Draft v0.1 · **License targets:** MIT (firmware/app), CERN-OHL-P (hardware/models)

An open-source, clip-on launcher module that measures launch RPM and launch angle for Beyblade X, reporting over BLE. Functional alternative to the official BattlePass with data it doesn't capture (IMU) and an open protocol.

> Naming note: "BattlePass" and "Beyblade" are Takara Tomy/Hasbro trademarks. Project name and repo must not use them except descriptively ("compatible with Beyblade X launchers").

---

## 1. Goals

- **Zero launcher modification (hard requirement):** the device touches only the launcher's built-in mount rail — nothing glued, pressed on, or altered. This is what keeps tournament use on the table.
- Measure peak launch RPM with accuracy comparable to the official BattlePass (±2% target)
- Measure launch angle relative to a calibrated stadium plane
- Capture rip motion profile (accel curve, smoothness/consistency score)
- Report over BLE to a Web Bluetooth app — no app store, no account
- Fully reproducible from the repo: ~$20 in parts, a 3D printer, basic soldering
- Non-devs can flash firmware from a browser (ESP Web Tools)

## 2. Non-Goals (v0.x)

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
| Spin sensor | QRE1113 IR reflective sensor, aimed at the launcher's exposed rotating spool through the BattlePass mount opening (same sensing approach as the official module — launcher stays stock) | $1 |
| Battery | 401030 LiPo 100mAh (XIAO onboard charging) | $4 |
| Switch | SPDT slide | $0.50 |
| Housing | PLA, clip-on to string launcher grip rail | ~$0.30 |

Fallback if stock spool geometry doesn't produce clean reflectivity pulses: a **removable** high-contrast sticker on the spool (peels off clean — still zero permanent modification, but weaker for tournament optics; avoid if stock geometry works).

### v0.2 — Adds IMU
| Part | Component | Est. cost |
|---|---|---|
| IMU | LSM6DSO 6-axis (I2C) on launcher body | $5–8 |

**Optical signal quality off stock spool geometry is the primary technical risk.** Pulses-per-rev and surface contrast through the mount opening are unknown until measured (M0a). Sensor distance is a solved problem by comparison — the mount rail fixes it. Characterize before any CAD.

### Small-batch path (later)
Custom PCB (ESP32-C3 module + bare LSM6DSO + QRE1113), JLCPCB assembly: ~$8–12/unit @ qty 50.

## 4. Firmware

- **Stack:** PlatformIO, Arduino framework on ESP32-C3
- **RPM:** QRE1113 reflectivity edges → ADC threshold with hysteresis (or comparator-style digital read) → interval timing. Pulses/rev is a config constant, TBD from M0a stock-geometry characterization; peak RPM = min interval during launch window. Launch end = no pulse for 250ms. Serial raw mode streams ADC samples for signal characterization.
- **IMU (v0.2):** LSM6DSO @ 416Hz over I2C, Madgwick fusion for orientation. Log accel/orientation ring buffer during rip; snapshot orientation at final spool pulse = release angle.
- **Calibration:** button-triggered 2s rest on stadium floor (or rim jig) → zero orientation to stadium plane. Store in NVS.
- **BLE:** GATT server, custom service. Notify characteristic pushes a launch record post-launch; second characteristic for live RPM stream (training mode).
- **Power:** deep sleep after 5 min idle, wake on motion (IMU interrupt) or button.

### Launch record (v1 schema)
```
{
  seq: uint16,
  peak_rpm: uint16,
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
- Clips to Takara Tomy string launcher grip rail (official BattlePass mount point)
- Two-part shell: sensor window (fixed QRE1113 aim through the mount opening) + electronics bay
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

- **M0a — Mount characterization:** physically characterize the BattlePass mount opening — what's visible through it, what rotates, surface features (spokes/ribs/color changes usable as optical marks), working distance. *Gates M0b.*
- **M0b — Sensor proof:** breadboard QRE1113 + ESP32, clean pulse train off a real launcher through the mount opening, serial output. *Go/no-go gate for everything else.*
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
5. Pulses-per-rev off stock spool geometry — TBD from M0a. Affects RPM math resolution and the glitch-filter floor.
6. Tournament legality is TO-discretion even at zero modification — most rulesets allow *official* accessories, not arbitrary attachments. Frame the device as training/casual-legal, tournament-pending; zero modification keeps the door open, it doesn't guarantee entry.
