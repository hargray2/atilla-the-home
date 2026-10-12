# ATILLA Mini 1.5.4 — No-card hardware acceptance (2026-10-11)

User installed 1.5.4 on ESP32-2432S028 CYD **without a microSD card inserted**; the Samsung 64GB FAT32 card remains in the 7-inch Waveshare.

Selected on-device logs:

```text
[I][app:151]: ESPHome version 2026.9.1 compiled on 2026-10-11 22:47:57 -0400
[I][app:158]: ESP32 Chip: ESP32 rev3.1, 2 core(s)
[S][binary_sensor]: 'Mini API Status' >> ON
[W][mini_sd:157]: No SD idle response (CMD0=FF)
[I][mini_weather:111]: Open-Meteo current + 3-day forecast updated (HTTP 3792 ms)
[W][component:419]: interval took a long time for an operation (3804 ms), max is 50 ms
[W][component:419]: display took a long time for an operation (276 ms), max is 270 ms
[I][safe_mode:154]: Boot seems successful; resetting boot loop counter
[S][sensor]: 'Mini Uptime' >> 64 s
[S][sensor]: 'Mini WiFi Signal' >> -39 dBm
```

## Acceptance

- **PASS:** No-card SD initialization fails safely with CMD0=FF; no card-mandatory boot path or crash observed.
- **PASS:** Home Assistant API connects; live Open-Meteo weather and 3-day forecast update; safe-mode boot counter resets; uptime and Wi-Fi telemetry continue.
- **NOT TESTED:** Known-file content read and CRC32 introduced in 1.5.4, since the card was absent. Previous 1.5.3 hardware test already proved root sector read with card inserted.
- **PERFORMANCE ISSUE:** Weather HTTP request consumes 3,792 ms and stalls the ESPHome interval for 3,804 ms. Main-loop blocking should be addressed before introducing SD graphics or other time-sensitive interactions. Display operation 276 ms vs 270 ms watchdog warning threshold is a smaller issue.

ESPHome CLI 'Loaded validated config cache, skipping validation' is not a device error. The device is nevertheless running firmware compiled at the timestamp shown. No Waveshare code or files changed.
