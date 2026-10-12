# ATILLA Mini 1.5.3 — CYD hardware acceptance (2026-10-11)

User flashed the credential-free-source-equivalent **1.5.3** build to the ESP32-2432S028 CYD and supplied these serial logs:

```text
[21:59:50.822][I][app:158]: ESP32 Chip: ESP32 rev3.1, 2 core(s)
[21:59:52.838][I][mini_sd:230]: SD probe: FAT32 READ-ONLY PASS, partition LBA=32768, volume=61040 MiB, sectors/cluster=64
[21:59:52.848][I][mini_sd:265]: ROOT FIRST SECTOR READ OK: 2 entries, first=SYSTEM~1
[22:00:20.733][I][mini_weather:111]: Open-Meteo current + 3-day forecast updated (HTTP 2357 ms)
[22:00:20.741][W][component:419]: interval took a long time for an operation (2368 ms), max is 50 ms
[22:00:21.014][W][component:419]: display took a long time for an operation (276 ms), max is 270 ms
[22:00:45.568][I][safe_mode:154]: Boot seems successful; resetting boot loop counter
```

## Confirmed

- CYD's GPIO18/23/19/5 software SPI bus can read the Samsung 64GB FAT32 card and its volume parameters.
- FAT32 first root-sector read succeeded with two visible entries and first short name `SYSTEM~1`; first-sector enumeration is **not** a complete directory listing.
- Open-Meteo 3-day JSON fetch succeeds on device using HA-sourced coordinates; recent logged latency 2357ms. An earlier sample was 3827ms; not a controlled comparison.
- LCD updates continue at 20MHz and 1s refresh with a 276ms warning, down from the earlier 337ms single sample (~18% shorter per operation, not a validated throughput benchmark).
- ESP32 rev3.1, strong -42 to -44 dBm Wi-Fi RSSI, ADC sensor updates, uptime reporting, successful stable boot.

## Not yet verified

- File-content reads and CRC, generic filesystem mount/FAT walking, asset browsing, SD writes or cache.
- Weather HTTP is still synchronous on main loop and generated a 2368ms interval warning. This is a performance issue, not a failed weather update.
- LCD rendering still exceeds warning threshold in this sample. One sample cannot prove consistent improvement.

## Next experiment (1.5.4)

Attempt bounded read-only CRC verification of the known card entry `SYSTEM~1/WPSETT~1.DAT`: 12 bytes, previously measured CRC32 `630105D1` on 7-inch ATILLA. Missing/changed reference file is *not* an SD failure. Keep existing hardware-confirmed 1.5.3 rollback; no weather transport redesign in same build.
