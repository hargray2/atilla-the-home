# ATILLA Mini 1.5.5 runtime acceptance + 1.5.6 touch latency experiment

## Verified on user's ESP32-2432S028 (1.5.5, ESPHome 2026.9.1)

```text
[I][app:151]: ESPHome version 2026.9.1 compiled on 2026-10-11 23:33:53 -0400
[I][app:158]: ESP32 Chip: ESP32 rev3.1, 2 core(s)
[W][mini_sd:195]: No SD idle response (CMD0=FF)
[I][mini_weather:138][mini_wx_https]: Async HTTPS 4328 ms status=200 result=OK
[I][mini_weather:948]: Open-Meteo real weather and 3-day forecast updated (background)
[W][component:419]: interval took a long time for an operation (277 ms), max is 270 ms
[I][safe_mode:154]: Boot seems successful; resetting boot loop counter
```

**Accepted:** async HTTPS successfully runs on a FreeRTOS worker with verified TLS, returning successful current weather and 3-day forecasts; the formerly 3-4 second main-loop interval stall does not appear in this log. No-card SD response remains safe; Wi-Fi excellent; normal boot. Touch responsiveness **not yet measured** by these logs. The 277ms interval warning occurs directly after weather completion, because the 100ms scheduler calls synchronous `mini_display.update()` for the newly dirty page. This is not evidence of another 277ms HTTPS stall.

## 1.5.6 isolated experiment

Full credential-free source: `experimental/atilla-mini/atilla-mini-1.5.6-touch-latency.yaml`.

- Change MIPI SPI `buffer_size` from **25%** (19,200 bytes, 4 rendering passes) to **50%** (38,400 bytes, 2 rendering passes) on the 320x240 8-bit display. The 320 native pixel dimension divides evenly into 2/4 but not 3; due to ESPHome `denominator()` selection a 33% or 40% setting would still select four passes. 50% is the first meaningful jump. Additional RAM cost ≈19,200 bytes.
- Preserve tested ST7789V 20MHz SPI, XPT2046 25ms touch polling, the async Open-Meteo worker, SD read-only probe, known file CRC, UI pages and pin mappings.
- Instrument touchscreen release timestamp and the first subsequent full-display update. On touch-driven frame completion, log `mini_touch_perf: Touch->LCD complete XX ms (render YY ms), heap=ZZ B`. This provides direct evidence for latency and remaining heap; display redraws remain synchronous and may still produce ESPHome loop warning.

**Risk:** classic ESP32 internal heap / contiguous allocation and async TLS memory peaks. This is an experiment, not a proven speedup until user tests; if boot/display/weather fail or heap becomes critically low, return to verified 1.5.5 (or 1.5.4). No file writes, no card requirement, no Waveshare changes. YAML structural parsing passed; full ESPHome compile and physical test pending. Private configured YAML must not be committed publicly.
