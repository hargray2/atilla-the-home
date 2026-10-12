# ATILLA Mini NEO 1.4.5 — Scope Edition

- Removed redundant `MISSION CONTROL / LIVE` Home heading and raised clock/date; home weather tiles expanded.
- Removed the ART/Pixel Forge drawing code and pixel vector. Replaced second-bank ART button with **SCOPE**, a real rolling free-heap line graph using the existing 2-second sampling history (60 samples; approximately two minutes). No invented telemetry.
- Kept centered pill buttons: bank 1 HOME/WX/ROOM/GAME, bank 2 SYS/SCOPE/FX/CTRL; preserved bank swipe and brightness/theme interaction.
- Weather values are genuine only when Home Assistant provides the configured `weather.forecast_home` attributes. Three-day high/low placeholders remain unavailable without sensor entities; no fabricated forecast values.
- Credential-free and locally configured private YAML files are downloadable in the corresponding conversation; full source is **not yet committed** here. Public/private YAML was syntax-parsed, not ESPHome-compiled or hardware-validated.
- No real HA device actions, no SD dependency, no Waveshare edits.
