# OpenRip

Open-source launch telemetry for Beyblade X launchers: XIAO ESP32-C3 + hall
sensor measuring launch RPM, reported over serial (M0) and later BLE.

## Commands (run from /firmware)

- `pio run` — build (default env is serial-only M0)
- `pio run -t upload` — flash over USB
- `pio device monitor` — serial output at 115200

## Deploying the app

GitHub Pages serves the `gh-pages` branch (https://moon-lite.github.io/OpenRip/).
After changing /app, republish from the repo root:
`git subtree split --prefix app -b pages-deploy && git push origin pages-deploy:gh-pages --force && git branch -D pages-deploy`
(An Actions workflow would be better; blocked until the gh token has `workflow` scope.)

## Conventions

- Pin assignments and tunables live in `firmware/include/config.h`.
- **SPEC.md is the source of truth** for schemas (launch record), milestones,
  and scope. Don't change wire formats or milestone scope without it.
