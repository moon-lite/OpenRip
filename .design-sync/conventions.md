# OpenRip design conventions

OpenRip is a **dark-first, industrial telemetry** aesthetic ("garage dyno"):
carbon-black surfaces, tachometer-orange accents, acid-green live signals,
uppercase letterspaced labels. Screens are instrument panels, not documents.

## Setup

No provider is required. Components style themselves via CSS custom
properties defined in `styles.css` (`:root` scope) — make sure that
stylesheet is loaded. **Always give the page a dark ground**: set
`background: var(--or-bg)` on the page/root container; components assume it
and look wrong on white.

Fonts load via the Google Fonts `@import` already inside `styles.css`
(Chakra Petch for display, IBM Plex Mono for data) — nothing to add.

## Styling idiom

There are **no utility classes**. Use the components' props for the
controls; for your own layout glue (containers, grids, spacing) write plain
CSS or inline styles using the tokens:

- Surfaces: `--or-bg` (page), `--or-bg-raise` (panels), `--or-line` (hairline borders)
- Ink: `--or-ink` (primary), `--or-ink-dim` (labels/captions)
- Signals: `--or-accent` (orange — brand, actions), `--or-live` (green — live data, success), `--or-danger`
- Type: `--or-font-display` (headings, buttons), `--or-font-mono` (all data/numerals)
- Spacing: `--or-space-xs/sm/md/lg/xl` (4/8/16/24/40px)
- `--or-clip`: the corner-clip polygon if a custom element should match Button's angular corners

Numerals are always mono with `font-variant-numeric: tabular-nums`. Labels
and micro-headers are uppercase with wide letterspacing (0.2–0.35em) in
`--or-ink-dim`.

## Components (window.OpenRipDesign)

`Button` (variant: primary | ghost | danger — one primary per screen),
`StatusChip` (online + label), `Gauge` (value/max/unit/size — the hero
readout, one per screen), `StatTile` (label + value; "—" for empty),
`HazardStripe` (section divider, use once), `Banner` (persistent notices),
`Wordmark` (top-left brand), `DataTable` (columns/rows/emptyText — logs and
history).

## Where the truth lives

Read `styles.css` (tokens at top, component classes below) before inventing
styles; per-component APIs are in each `<Name>.d.ts`.

## Idiomatic screen skeleton

```jsx
const { Wordmark, StatusChip, Button, HazardStripe, Gauge, StatTile } = window.OpenRipDesign;

<div style={{ background: "var(--or-bg)", minHeight: "100vh", padding: "var(--or-space-lg)" }}>
  <header style={{ display: "flex", justifyContent: "space-between", alignItems: "flex-end", paddingBottom: "var(--or-space-md)" }}>
    <Wordmark subtitle="Launch Telemetry · v0.1" />
    <div style={{ display: "flex", gap: "var(--or-space-sm)" }}>
      <StatusChip online label="Linked" />
      <Button variant="primary">Disconnect</Button>
    </div>
  </header>
  <HazardStripe />
  <main style={{ display: "flex", gap: "var(--or-space-xl)", alignItems: "center", padding: "var(--or-space-xl) 0" }}>
    <Gauge value={8450} />
    <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 1 }}>
      <StatTile label="Session Best" value="11,240" />
      <StatTile label="Launches" value="14" />
    </div>
  </main>
</div>
```
