# ATILLA Mini 1.5.2 — Hardware acceptance (2026-10-11)

User installed the 1.5.2 firmware and supplied runtime observations from the ESP32-2432S028 CYD:

```text
[S][app:158]: ESP32 Chip: ESP32 rev3.1, 2 core(s)
[S][binary_sensor]: 'Mini API Status' >> ON
[I][mini_sd:228]: SD probe: FAT32 READ-ONLY PASS, partition LBA=32768, volume=61040 MiB, sectors/cluster=64
[I][mini_weather:111]: Open-Meteo current + 3-day forecast updated
[W][component:419]: interval took a long time for an operation (3827 ms), max is 50 ms
[W][component:419]: display took a long time for an operation (337 ms)
```

**Confirmed:** proposed CYD SD GPIO18/23/19/5 software SPI path responds using CMD0/8/55/41/58/17; FAT32 MBR and boot-sector geometry can be read without writes. The card reports 61,040 MiB (= about 59.6 GiB). Open-Meteo HTTPS returns parseable current weather and 3-day forecasts with HA-sourced `zone.home` coordinates. Native ESP-IDF boots on ESP32 rev3.1; HA API status ON.

**Not confirmed:** generic FatFs mount, filesystem/directory enumeration, arbitrary file-content reads, CRC verification, writes, permanent caching, or absolute accuracy of weather values. These must not be claimed as tested.

**Performance:** ESPHome `http_request.get` executes on the main loop and is responsible for the approximately 3.8-second interval stall when the remote forecast is fetched. The ST7789V display shows ~337 ms operations at existing 500ms refresh/25% frame buffer. No evidence of crash or card corruption in supplied excerpt. Maintain read-only SD policy, HTTPS certificate verification, and rollback copy of working 1.5.2.
