# design-sync notes — OpenRip

- The DS package lives at `design/` (`@openrip/design`), created 2026-07-04
  specifically to codify the shipped vanilla app's look (`app/style.css` is
  the visual source of truth the DS mirrors — keep them in agreement).
- No dist entry in node_modules (own-repo): always pass
  `--entry ./design/dist/index.js` and `--node-modules design/node_modules`.
- `dist/styles.css` is built self-contained: the package build **prepends**
  `src/tokens.css` (same-package tokens can't ship via `tokensGlob`, which
  requires a separate `tokensPkg` in node_modules). Don't re-add an
  `@import "./tokens.css"` to `src/styles.css` — it broke the bundle closure
  once already ([CSS_IMPORT_MISSING]).
- Fonts are Google-Fonts remote (`@import` at top of tokens.css) →
  `[FONT_REMOTE]` info line is expected, not a problem.
- The repo root `.gitignore` has a global `dist/` rule (predates the DS), so
  `design/dist/` is untracked — run `npm run build --prefix design` before
  any converter run.
- `overrides.StatTile.cardMode = column`: the StatsGrid story is wider than
  a grid cell ([GRID_OVERFLOW] fix, presentation-only).
- Components assume a dark ground; all authored previews wrap stories in a
  `var(--or-bg)` container. Do the same for any new preview.

## Known render warns

(none — 8/8 clean, no triaged warns)

## Re-sync risks

- The DS mirrors `app/style.css` by hand — if the app's CSS evolves without
  touching `design/`, the sync happily re-verifies a stale look. Check the
  two agree when the app has changed.
- Google Fonts loads at runtime in previews; a network-less environment
  will render fallback fonts and could fail grading spuriously.
- Playwright chromium was installed to the default cache
  (`~/Library/Caches/ms-playwright`, headless-shell v1228) — a fresh machine
  needs `npx playwright install chromium` from `.ds-sync/`.
- Nothing was partially verified: all 8 components authored + graded good.
