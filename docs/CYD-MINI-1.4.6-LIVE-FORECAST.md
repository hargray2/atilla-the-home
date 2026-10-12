# ATILLA Mini NEO 1.4.6 — Real Open-Meteo weather

The corresponding private and credential-free complete YAML files are available as conversation downloads.

- Uses Home Assistant `zone.home` latitude/longitude attributes over ESPHome native API (no coordinates hard-coded in the repository).
- ESPHome `http_request` requests **Open-Meteo** current temperature/RH/wind and daily high/low/weather code for 3 days, in Fahrenheit/mph, with automatic time zone.
- Parses all returned values into local globals together and preserves last successful data on request failure.
- Startup polls every 30 seconds until first success, then refreshes every 45 minutes; does not require forecast template sensors or HA service actions.
- Keeps 1.4.5's four centered pill buttons per bank, SCOPE, simulated controls, arcade, theme, and brightness. Radar and ART remain removed. No SD card needed.
- Needs Home Assistant to expose home-zone coordinates and allow outbound HTTPS from ESP32. If unavailable, display placeholders instead of fabricated data.
- This variant uses ESPHome HTTP with a 4KiB captured response and an ESP-IDF network stack; monitor heap and touch responsiveness for regressions.
- Basic YAML tag parsing passed. **ESPHome compilation and hardware validation have not been performed.** Never commit private credentials; Waveshare branch untouched.
