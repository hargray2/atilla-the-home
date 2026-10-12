# ATILLA 4.7.9z — hardware-validated read-only manifest and file CRC

Hardware: Waveshare ESP32-S3-Touch-LCD-7, ESP32-S3 rev0.2, 800x480 / GT911, PSRAM 8 MiB. ESPHome 2026.9.1 build timestamp 2026-10-11 22:30:45 -0400. Observations are from the user's ~2 minute startup log on 2026-10-11, with private network identifiers omitted.

## Combined acceptance

| Check | Hardware observation |
|---|---|
| Boot + encrypted Home Assistant API | PASS, connected and handshake; no reboot observed |
| SDHC/FAT32 and 12-byte legacy CRC | PASS; read-only init, CRC32 `630105D1` |
| Single FatFs mount/unmount | PASS, `f_mount=0` (~138ms), `f_unmount=0` |
| `/ATILLA` discovery | PASS: `VERIFY.BIN` 4096 B; `SETT01.TST` 32 B; `manifest.json` 168 B; `icons/` directory; `complete=YES` |
| Bounded CRC two passes | PASS, 4096 B / 16 chunks each, CRC32 `5ACE03B3` |
| Reusable asset reader | PASS, 4096 B / 16 chunks, observed reference CRC32 `5ACE03B3` |
| Manifest parser + declared image | PASS, manifest describes one 70-byte image; on-card size / CRC check passed |
| Combined manifest worker | **`COMBINED MANIFEST READ-ONLY PASS discovery=PASS crc=PASS asset=PASS manifest=PASS NO WRITES`** |
| SD worker minimum stack free | 17,252 B of 24,576 B task stack |
| Weather current + forecast | PASS, HTTP 200; 542 B + 1118 B |
| CARTO map | PASS, all six HTTPS tiles decoded/displayed |
| Radar | PASS, all six PNG worker tiles downloaded/decoded/filtered/displayed |
| Loaded free memory after all tiles | ~4,895 kB PSRAM / ~100 kB internal heap; largest internal block ~31 kB |
| Uptime and ambient mode | >123 seconds; ambient activated after two minutes |

## Boundaries and follow-ups

- The manifest parser and reader have passed one real-card read-only test. This **does not** prove long soak stability, hostile manifest fuzz resistance, SD unplug/reinsertion behavior, concurrent storage clients or bad-sector recovery.
- CRC32 detects unintended data changes and compares manifest-declared integrity, **not** cryptographic authenticity. Only one tiny 70-byte image has been tested; larger files, multiple entries and JPEG formats remain unvalidated on hardware.
- **No SD writes attempted** in the combined worker. The unrelated ESPHome `preferences: Writing 1 items` log reflects preferences persistence, not evidence of SD writes. Older explicitly gated write experiments still exist in the storage module; do not enable them.
- **No SD image displayed.** All visible map and radar tiles remain network-sourced; file-to-LVGL buffer lifetime and display handoff are future development.
- Main loop warnings in excerpt include 489 ms interval, **1,444 ms API operation**, and later radar refresh intervals up to **492 ms**, warranting responsive UI investigation. The startup log itself does not prove absence of visible glitches.
- Build-flag deprecation: the firmware TXT was updated to use `esphome.build_flags`, but the runtime log says ESPHome **skipped config validation cache**. The supplied log is not compiler validation output, so absence of the deprecation warning is **not confirmed** by this log.
- FatFs cooperative I/O deadlines do not cancel a blocked hardware read.
- Keep complete 4.7.9y private firmware as known-good rollback, alongside newly validated 4.7.9z; never commit private `waveshare.yaml`, credentials, location or Wi-Fi identifiers.
