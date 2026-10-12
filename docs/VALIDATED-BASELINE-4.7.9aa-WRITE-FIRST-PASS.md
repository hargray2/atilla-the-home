# ATILLA 4.7.9aa — first SD graphics-cache write acceptance (one startup)

Hardware: Waveshare ESP32-S3-Touch-LCD-7, ESP32-S3 rev0.2, ESPHome 2026.9.1. User-provided device log compiled 2026-10-11 22:57:35 -0400; startup observed ~23:01 to 23:03 local on 2026-10-11. Excludes Wi-Fi, location, credentials and personal identifiers.

## Verified on hardware

| Gate | Observed result |
|---|---|
| OTA-booted application + API | PASS, encrypted handshake, no reboot observed in supplied excerpt |
| SDHC/FAT32 init | PASS |
| Legacy raw 12-byte file | PASS, CRC32 `630105D1` |
| FatFs mount | PASS, return 0 (~139 ms) |
| SDHC CSD vs partition end | PASS, verified card capacity and partition end |
| Cache write transaction | **PASS: `CACHE WRITE PILOT PASS slot=1 bytes=70 crc32=F60D2E27 writes_disabled=YES`** |
| Overall pilot | **PASS: `CACHE WRITE PILOT OVERALL PASS`** |
| FatFs unmount | PASS, return 0 (~1 ms) |
| Worker stack minimum free | 16,920 bytes (24,576-byte worker stack) |
| Weather current/forecast | PASS, HTTP 200, 542 B / 1,119 B |
| CARTO | PASS all six map HTTPS requests and LVGL displays |
| Radar | PASS all six tile PNG downloads, decode, filtering, LVGL displays |
| Memory after tile load | ~4,895 kB PSRAM; ~100 kB free internal heap, 52 kB largest internal block at end |
| Ambient mode | Activated after 2 minutes; no reboot in excerpt |

## Acceptance boundary

This log provides **first-write hardware success** of the explicit pilot, not a second-boot immutable read-only cache validation, physical SD file inspection or power-loss endurance result. Do **not** claim those gates are passed until separately tested.

The successful pilot writes one embedded 70-byte 1x1 PNG to the fixed immutable `/ATILLA/CACHE/C0001.PNG` path, via a temporary stage, sync, CRC readback and rename. The on-card output path is inferred from the current code, and was **not independently inspected** on a PC in this log. The pilot does not fetch HTTPS images, replace the dashboard art, render SD assets to LVGL, change the manifest or provide a production write service.

Potential unresolved issues:
- FAT32 has no journaling. Even a staged/synced/verified transaction is not robust against removal/reset during writes.
- First boot log alone cannot prove pre-existing files were preserved or no file-system corruption occurred.
- Main-loop interval warnings of ~104, 182, 331, 341, 481, 489 and 491ms remain; interactive touch responsiveness requires separate testing.
- An early API connection closed (`CONNECTION_CLOSED errno=128`) but subsequent API session remained connected; investigate if recurrent.
- The storage component's `dump_config` text still inaccurately says 'READ-ONLY probe; no FATFS, no writes' even with the explicit write pilot mode; update diagnostic wording before promotion.
- ESPHome log reused its validated configuration cache, so absence of the old `platformio_options.build_flags` warning is not separately verified from this startup log.

## Remaining pilot gate before merging

**Restart/power-cycle the existing 4.7.9aa device with normal stable power** (no recompilation or additional flash) and supply the storage log. Expected:
`CACHE PILOT EXISTING PASS slot=1 NO WRITES`
and
`CACHE WRITE PILOT OVERALL PASS`,
followed by clean unmount and healthy startup. The implementation should use read-only validation against `C0001.PNG`, avoiding further writes. If `CACHE PILOT` fails, do not reflash or repeatedly retry; preserve the SD and log. A full card backup remains essential before additional write experiments.

Keep complete private 4.7.9z fallback. Firmware and device identifiers remain private and are never checked into this public repo.
