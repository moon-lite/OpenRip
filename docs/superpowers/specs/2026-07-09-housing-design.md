# M1 Housing Design — Blender + MCP Workflow

**Status:** Approved (brainstorming session 2026-07-09) — implementation not yet started.

## Context

M0a (mount characterization) and M0b (sensor proof) are complete: the winder
launcher's factory optical encoder is confirmed readable through its
top-face window with zero modification, and a breadboard build has proven
clean RPM readings end-to-end, including presence-gated launch detection via
a microswitch on the launcher's spring plunger. See
`docs/sensor-characterization.md` for the validated data.

M1 is next per `SPEC.md` §8: printed housing, battery power, BLE launch
records. This spec covers only the **housing** piece of M1. No CAD exists
yet — `models/` currently holds only `LICENSE` and a stub `README.md`, and
that stub's description ("clip-on shell for the string launcher grip rail")
is stale/incorrect: `SPEC.md` §6 correctly specifies a **top-face** mount
over the encoder window, registering against top-face geometry, not the
grip rail. This work corrects that stub as part of landing the real design.

## Goals

- Design a two-part printable shell that mounts non-destructively on the
  winder launcher's top face and holds:
  - the sensor breakout module (VCC/GND/AO/DO, onboard LM393 comparator —
    the part actually validated at M0b, not the bare TCRT5000 originally
    speced in the BOM) positioned over the ~5×5mm encoder window
  - the bey-presence microswitch on an adjustable carriage (slot + M2
    screw/heat-set insert), as already described in `docs/build-guide.md`
  - the Seeed XIAO ESP32-C3
  - the 401030 LiPo battery
  - an SPDT power slide switch
- Drive the modeling through Claude Code via a Blender MCP connection, using
  a phone-captured 3D scan of the launcher's top face as the registration
  reference surface, so the mount fits the launcher's real geometry rather
  than estimated dimensions.
- Deliver STEP + STL + Blender source (`.blend`), updating `SPEC.md` §6's
  deliverable format away from Fusion's `.f3d` (no longer applicable since
  we're not using Fusion 360).

## Non-goals

- Finalizing the exact registration/clip geometry in this document — that's
  a first-CAD-iteration decision, made after the scan data exists, not
  something to lock in blind.
- IMU mounting — that's M4/v0.2 scope, not this milestone.
- Any launcher modification. Zero-modification is a hard requirement
  (`SPEC.md` §1) carried over unchanged: the mount must be a removable,
  non-destructive friction-fit/clip — nothing glued, nothing pressed in
  permanently, nothing altered on the launcher itself.

## Decisions from brainstorming

- **Geometry source:** phone 3D scan (LiDAR/photogrammetry) of the bare
  launcher top face, exported as a mesh (OBJ/STL).
- **Sensor footprint:** design around the breakout module already wired and
  validated at M0b, not the bare TCRT5000 from the original BOM. (BOM/
  wiring.md's "buy bare sensors, not modules" note is now superseded for
  this build and should be revisited/updated separately when this housing
  work lands.)
- **Print setup:** the builder has their own FDM printer, so a fast
  print-and-check iteration loop is available and expected to be used.
- **Modeling role:** hands-off — Claude drives the Blender modeling via
  MCP; the builder reviews rendered screenshots/exports and handles the
  physical scan capture and print-fit testing.
- **Tooling:** Blender + a Blender MCP server for modeling (chosen over
  FreeCAD+MCP and OpenSCAD specifically because it handles an imported
  organic scan mesh — for boolean registration fits — far better than
  either alternative). FreeCAD is used only as a narrow, scripted,
  non-MCP conversion bridge at final export time (STL → STEP), since
  Blender's native export path is STL and STEP is a required deliverable.

## Pipeline

```
Phone scan (bare launcher top face, no breadboard in the way)
        │  export OBJ/STL
        ▼
Blender  ──MCP──▶  Claude scripts the model:
  - import scan as a fixed reference (never modified/exported itself)
  - build shell geometry as separate mesh objects, booleaned
    against the scan surface for a snug registration fit
  - position pockets: sensor module, XIAO, battery, switch carriage
  - render viewport screenshots after each major step for review
        │  export STL per part
        ▼
FreeCAD (headless, scripted) ──▶ STL → STEP conversion
        │
        ▼
Print → test-fit on the real launcher → adjust dimensions → reprint
        │
        ▼
Final deliverables committed: STEP + STL + Blender source
```

The scan mesh itself is a large binary file and is never committed — see
Repo conventions below.

## Components

- **Sensor carrier** — holds the sensor breakout module's PCB centered over
  the launcher's ~5×5mm window, at whatever standoff the module's photo-dome
  height requires (pinned down by directly measuring the module during
  modeling, not assumed up front). Also carries the **adjustable switch
  carriage**: a slot + M2 screw (or heat-set insert) letting the microswitch
  slide toward/away from the plunger and lock once calibrated, per the
  existing calibration procedure in `docs/build-guide.md`.
- **Electronics bay** — holds the XIAO ESP32-C3, the 401030 LiPo battery,
  and the SPDT power slide switch. Includes a USB-C cutout (charging/
  reflashing without disassembly) and a slide-switch cutout (actuator tab
  reachable from outside).
- **Joining the two shell halves** — snap-fit cantilever tabs by default,
  not screws, so the enclosure opens for battery access without needing
  fasteners. The M2 screw/insert is reserved solely for the switch carriage
  lock.
- **Mounting to the launcher** — a friction-fit/spring-clip that registers
  against whatever real lip or edge the scan reveals on the top face.
  Exact geometry is a first-CAD-iteration decision (see Non-goals). Must
  stay fully removable and non-destructive — no adhesive, nothing pressed
  permanently into the launcher.
- **Print approach** — PLA, ~1.6–2mm walls, split lines and overhangs
  chosen to preserve the "no supports" goal from `SPEC.md` §6.

## Repo & tooling conventions

- `models/scans/` — gitignored. Holds raw phone-scan exports. Never
  committed — large, binary, and not useful in git history.
- `models/housing/` — the actual deliverables: the Blender `.blend`
  source, exported `.stl` per part, and the converted `.step`. Satisfies
  `SPEC.md` §6's "STEP + STL + source" once that section's format note is
  updated from `.f3d` to `.blend`.
- One-time environment setup (implementation-plan concern, noted here for
  completeness): Blender installed locally, the Blender MCP addon enabled,
  and its server running so Claude Code can connect to a live Blender
  session.
- Doc updates bundled into this work: fix `models/README.md`'s stale
  "grip rail" description; update `SPEC.md` §6's deliverable format; give
  `docs/build-guide.md`'s M1 section real modeling-workflow content once
  the pipeline exists (same pattern already used for M0b).

## Print-and-validate loop / acceptance criteria

At least one print-and-adjust cycle is expected — a scan-based fit is a
strong starting point, not a guaranteed first-try perfect one. Print the
sensor carrier first (smaller, most fit-critical) before committing to the
full electronics bay, so a bad registration fit is caught cheaply.

The M1 housing is done when, on a printed and assembled unit:

- The shell clips onto the launcher's top face snugly, without wobble, and
  comes off cleanly — no tools, no residue, no interference with normal
  winding, bey-seating, or grip.
- With the housing mounted, a raw-mode ADC check (same procedure as M0b)
  still shows a clean two-level signal — confirms the housing didn't move
  the sensor off its validated working distance.
- The switch carriage calibration procedure in `docs/build-guide.md`
  passes: `ARMED` with a bey seated, `IDLE` with it removed, no resistance
  to seating.
- USB-C and the power switch are reachable without opening the shell; the
  two halves snap together and reopen cleanly for battery access.
- A real rip-cord launch with the housing mounted produces a launch report
  matching the bench validation (peak RPM in the expected 5,000–12,000
  range, plausible pulse count).

## Open questions (deferred to implementation)

- Exact top-face registration/clip geometry — decided from the scan data
  in the first CAD iteration, not before.
- Blender → STEP export path — confirm at implementation time whether a
  native Blender STEP export addon is usable, or whether the FreeCAD
  conversion bridge described above is needed.
- Sensor module standoff/orientation inside its pocket — pinned down by
  direct measurement of the physical module during modeling.
