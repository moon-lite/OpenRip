# open=rip design conventions

open=rip is a **light, airy, editorial** aesthetic — a clean zine, not a
dashboard. Paper-white ground, ink typography, hairline rules instead of
boxes, one teal accent for live data and the brand mark, italics for
captions and asides. Screens should read like a well-set magazine page
about your launches.

## Setup

No provider is required. Components style themselves via CSS custom
properties defined in `styles.css` (`:root` scope) — make sure that
stylesheet is loaded. **Give the page a paper ground**: set
`background: var(--or-bg)` on the page/root container; components assume a
light ground and look wrong on dark.

The single family (Instrument Sans, with true italics) loads via the Google
Fonts `@import` already inside `styles.css` — nothing to add. Hierarchy
comes from weight and rules, not color: 700 for numerals/headlines, 600 for
buttons, 500 for labels, 400 for body, *italic* for captions and notes.

## Styling idiom

There are **no utility classes**. Use the components' props for the
controls; for your own layout glue (containers, grids, spacing) write plain
CSS or inline styles using the tokens:

- Surfaces: `--or-bg` (paper), `--or-bg-raise` (white), `--or-line` (hairlines), `--or-line-strong` (ink rules)
- Ink: `--or-ink` (primary), `--or-ink-dim` (labels/captions)
- Signals: `--or-accent` (teal — brand, live data, links), `--or-accent-wash` (faint teal fill), `--or-danger` (coral — destructive only)
- Type: `--or-font` (the only family — vary weight and italic, not face)
- Spacing: `--or-space-xs/sm/md/lg/xl` (4/8/16/24/40px)
- Corners: `--or-radius-pill` (controls are pills), `--or-radius` (12px, rare — most surfaces are unboxed)

Prefer **rules over boxes**: separate sections with `Divider` or a 1px
`--or-line` border, not cards. Numerals get
`font-feature-settings: "tnum"` so columns align. Labels are small,
lowercase, `--or-ink-dim`. Generous whitespace — when in doubt, add space,
not decoration.

## Components (window.OpenRipDesign)

`Button` (variant: primary | ghost | danger — primary is the solid ink
pill, one per screen), `StatusChip` (online + label; hairline pill),
`Gauge` (value/max/unit/size — the hero readout, one per screen),
`StatTile` (label + value; top-rule index block, "—" for empty), `Divider`
(hairline ink rule), `Banner` (italic margin note with teal left rule),
`Wordmark` (the lowercase open=rip mark, top-left), `DataTable`
(columns/rows/emptyText — hairline rows, no box).

## Where the truth lives

Read `styles.css` (tokens at top, component classes below) before inventing
styles; per-component APIs are in each `<Name>.d.ts`.

## Idiomatic screen skeleton

```jsx
const { Wordmark, StatusChip, Button, Divider, Gauge, StatTile } = window.OpenRipDesign;

<div style={{ background: "var(--or-bg)", minHeight: "100vh", padding: "var(--or-space-xl)" }}>
  <header style={{ display: "flex", justifyContent: "space-between", alignItems: "flex-end", paddingBottom: "var(--or-space-md)" }}>
    <Wordmark subtitle="launch telemetry · vol 0.1" />
    <div style={{ display: "flex", gap: "var(--or-space-sm)", alignItems: "center" }}>
      <StatusChip online label="Linked" />
      <Button variant="primary">Disconnect</Button>
    </div>
  </header>
  <Divider />
  <main style={{ display: "flex", gap: "var(--or-space-xl)", alignItems: "center", padding: "var(--or-space-xl) 0" }}>
    <Gauge value={8450} unit="rpm · live" />
    <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: "var(--or-space-lg)" }}>
      <StatTile label="session best" value="11,240" />
      <StatTile label="launches" value="14" />
    </div>
  </main>
</div>
```
