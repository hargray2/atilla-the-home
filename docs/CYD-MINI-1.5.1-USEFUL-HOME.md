# ATILLA Mini NEO 1.5.1 — Useful Home

User feedback: the HOME pill on the Home page was a no-op, and the continuously extending colored bar above navigation had no meaningful state.

## UI changes

- On the Home page only, the first Bank 1 pill reads **SYNC**, calling `fetch_real_weather` on tap if not already running.
- On other Bank 1 pages the first pill remains **HOME**, providing the usual one-tap return.
- Removed the animated bar (`int beat=(millis()/220)%12; ... it.filled_rectangle(...)`) outright.
- Added real data freshness text (`WX SYNCING...`, `WX UPDATED Xm AGO` or `WX NOT READY`) to Home based on the HTTP script state and `wx_last_update` timestamp.
- Added `SD OK` when the experimental read-only FAT32 probe has succeeded.
- Moved the home weather cards down slightly to accommodate genuine status information.
- Kept centered pills, 2 navigation banks, all pages, Open-Meteo weather, and the experimental SD probe unchanged.

Downloadable private/credential-free 1.5.1 YAMLs are supplied in the conversation. **The source has not been committed to GitHub**; this file is documentation only. These files passed YAML parsing; full ESPHome build and device behavior are not yet verified.
