# ATILLA 4.7.9aa — write and reboot-persistence hardware acceptance

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

The first log provides **first-write hardware success** of the explicit pilot. A second separate reboot log from the same compiled firmware subsequently verified the immutable cached PNG **without writing it again**. Neither log establishes physical SD file inspection on a PC or power-loss endurance.

The successful pilot writes one embedded 70-byte 1x1 PNG to the fixed immutable `/ATILLA/CACHE/C0001.PNG` path, via a temporary stage, sync, CRC readback and rename. The on-card output path is inferred from the current code, and was **not independently inspected** on a PC in this log. The pilot does not fetch HTTPS images, replace the dashboard art, render SD assets to LVGL, change the manifest or provide a production write service.

Potential unresolved issues:
- FAT32 has no journaling. Even a staged/synced/verified transaction is not robust against removal/reset during writes.
- Neither startup log independently checks all pre-existing files or filesystem structural integrity; a full card backup is still necessary.
- Main-loop interval warnings of ~104, 182, 331, 341, 481, 489 and 491ms remain; interactive touch responsiveness requires separate testing.
- An early API connection closed (`CONNECTION_CLOSED errno=128`) but subsequent API session remained connected; investigate if recurrent.
- The tested firmware's `dump_config` still said 'READ-ONLY probe; no FATFS, no writes'. Subsequent repository code corrects this inaccurate diagnostic, but that correction was not part of the on-device binary.
- ESPHome log reused its validated configuration cache, so absence of the old `platformio_options.build_flags` warning is not separately verified from this startup log.

## Second-boot persistence gate — PASS

Same ESPHome 2026.9.1 firmware build timestamp (2026-10-11 22:57:35 -0400); user reboot log from 2026-10-11 approximately 23:23–23:25 local.

| Second-boot check | Result |
|---|---|
| Read-only FAT32 initialization and legacy file | PASS; unchanged CRC32 `630105D1` |
| FatFs read-only mount | PASS; mount=0, approximately 139 ms |
| Previously committed cached image | **`CACHE PILOT EXISTING PASS slot=1 NO WRITES`** |
| Overall pilot | **`CACHE WRITE PILOT OVERALL PASS`** |
| Clean FatFs unmount | PASS, result=0, approximately 1 ms |
| SD worker stack minimum free | 16,960 B |
| Weather current/forecast | PASS HTTP 200 |
| CARTO map tiles | PASS, all six HTTPS and LVGL displays |
| Radar PNG tiles | PASS, all six HTTPS/decode/filter/LVGL displays |
| Free RAM at end | ~4,895 kB PSRAM, ~100 kB internal heap |
| Runtime | More than two minutes without a reboot in supplied excerpt |

The second boot confirms **data persisted and was verified using the existing-file read-only path**; it does not mean the FAT32 filesystem itself is journaling-safe, nor does it validate power-loss recovery. A `preferences: Writing 1 items` message is normal ESPHome persistence and is not an SD file write. Wi-Fi and interval delays (including an observed 848 ms Wi-Fi operation) remain separate responsiveness concerns.

**The explicit pilot's first-write + repeat-boot hardware release gate is satisfied.** Complete private 4.7.9z firmware and a full card backup remain important before any further writes.

Keep complete private 4.7.9z fallback. Firmware and device identifiers remain private and are never checked into this public repo.
