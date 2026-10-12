# ATILLA Mini 1.5.7 — Screen Protection Hardware Acceptance (2026-10-11/12)

User compiled ATILLA Mini 1.5.7 under ESPHome 2026.9.1 at 2026-10-11 23:51:52 -0400 and supplied physical CYD runtime logs.

## Confirmed on hardware

```
[I][app:158]: ESP32 Chip: ESP32 rev3.1, 2 core(s)
[W][mini_sd:196]: No SD idle response (CMD0=FF)
[I][mini_weather:139][mini_wx_https]: Async HTTPS 3959 ms status=200 result=OK
[I][mini_weather:1016]: Open-Meteo real weather and 3-day forecast updated (background)
[W][component:419]: interval took a long time for an operation (202 ms), max is 200 ms
[I][safe_mode:154]: Boot seems successful; resetting boot loop counter
[I][mini_idle:1050]: Anti-retention saver after 60s; low light
[S][light]: 'Mini Backlight' >> ON
[S][light]:   Brightness: 8%
[I][mini_idle:1033]: Screen off after 120s of inactivity
[S][light]: 'Mini Backlight' >> OFF
[S][sensor]: 'Mini Uptime' >> 302 s
[S][sensor]: 'Mini WiFi Signal' >> -40 dBm
```

**PASS:** Background weather fetch and parse; 60-second low-brightness saver; 120-second backlight off; normal Wi-Fi, ambient-light and uptime sensors continue while display is off; safe-mode boot check passed. SD absent as expected (Samsung card remained in Waveshare). No unexpected crash shown.

**Unverified:** The first-touch wake does not trigger an underlying button (no wake touch logs supplied); successful second scheduled 10-minute weather update (logs show only first ~5 min); 1.5.7 SD known-file CRC when card inserted. Test wake action and observe second forecast update before claiming full acceptance.

**Performance:** 202 ms interval warning still occurs after weather result; the ~3.96 s HTTPS work was background and did not yield a comparable ~4 s interval warning. Prior 1.5.4 warning 3804 ms; 1.5.5 277 ms, but individual samples are not normalized. Further display optimizations optional; avoid risking current working hardware checkpoint without measurable gains.

**Important:** LCD is ST7789V TFT, so off-backlight idle protection is beneficial for retention, power and backlight lifespan; this log does not prove the LCD controller is fully powered down.

Branch scope: CYD Mini only; Waveshare 7-inch firmware unchanged.
