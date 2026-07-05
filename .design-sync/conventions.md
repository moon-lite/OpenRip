# OpenRip design conventions

OpenRip is a **clean, bold, dark-first** telemetry aesthetic: carbon-black
surfaces, one strong accent (tachometer orange), acid-green for live data,
generous weight contrast in a single typeface. Screens are calm instrument
panels — bold numbers, quiet labels, no ornament.

## Setup

No provider is required. Components style themselves via CSS custom
properties defined in `styles.css` (`:root` scope) — make sure that
stylesheet is loaded. **Always give the page a dark ground**: set
`background: var(--or-bg)` on the page/root container; components assume it
and look wrong on white.

The single brand font (Outfit) loads via the Google Fonts `@import` already
inside `styles.css` — nothing to add. Weight does the hierarchy work:
800 for hero numerals/wordmark, 700 for buttons/values, 600 for labels,
400–500 for body.

## Styling idiom

There are **no utility classes**. Use the components' props for the
controls; for your own layout glue (containers, grids, spacing) write plain
CSS or inline styles using the tokens:

- Surfaces: `--or-bg` (page), `--or-bg-raise` (panels), `--or-line` (hairline borders)
- Ink: `--or-ink` (primary), `--or-ink-dim` (labels/captions)
- Signals: `--or-accent` (orange — brand, actions), `--or-live` (green — live data, success), `--or-danger`
- Type: `--or-font` (the only family — vary weight, not face)
- Spacing: `--or-space-xs/sm/md/lg/xl` (4/8/16/24/40px)
- Corners: `--or-radius` (10px — buttons, banners), `--or-radius-lg` (14px — tiles, tables)

Numerals get `font-feature-settings: "tnum"` so columns align. Labels are
small, semibold, uppercase with mild letterspacing (0.06–0.08em) in
`--or-ink-dim` — never wide-tracked.

## Components (window.OpenRipDesign)

`Button` (variant: primary | ghost | danger — one primary per screen),
`StatusChip` (online + label; pill), `Gauge` (value/max/unit/size — the hero
readout, one per screen), `StatTile` (label + value; "—" for empty),
`Divider` (accent rule fading right, use once per screen), `Banner`
(persistent notices), `Wordmark` (top-left brand), `DataTable`
(columns/rows/emptyText — logs and history).

## Where the truth lives

Read `styles.css` (tokens at top, component classes below) before inventing
styles; per-component APIs are in each `<Name>.d.ts`.

## Idiomatic screen skeleton

```jsx
const { Wordmark, StatusChip, Button, Divider, Gauge, StatTile } = window.OpenRipDesign;

<div style={{ background: "var(--or-bg)", minHeight: "100vh", padding: "var(--or-space-lg)" }}>
  <header style={{ display: "flex", justifyContent: "space-between", alignItems: "flex-end", paddingBottom: "var(--or-space-md)" }}>
    <Wordmark subtitle="Launch Telemetry · v0.1" />
    <div style={{ display: "flex", gap: "var(--or-space-sm)", alignItems: "center" }}>
      <StatusChip online label="Linked" />
      <Button variant="primary">Disconnect</Button>
    </div>
  </header>
  <Divider />
  <main style={{ display: "flex", gap: "var(--or-space-xl)", alignItems: "center", padding: "var(--or-space-xl) 0" }}>
    <Gauge value={8450} />
    <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: "var(--or-space-sm)" }}>
      <StatTile label="Session Best" value="11,240" />
      <StatTile label="Launches" value="14" />
    </div>
  </main>
</div>
```
