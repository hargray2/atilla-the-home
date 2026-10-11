# ATILLA 4.7.9y combined read-only asset reader — hardware acceptance

Hardware: Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2. Firmware compiled 2026-10-11 02:41:26 -0400 on ESPHome 2026.9.1. User-provided log on 2026-10-11.

## Verified startup outcomes

| Check | Outcome |
|---|---|
| ESPHome boot and encrypted API handshake | PASS |
| Raw SDHC/FAT32 init | PASS |
| Legacy 12-byte file read CRC | PASS: 630105D1 |
| FatFs mount / unmount | PASS: f_mount=0 (138ms), f_unmount=0 |
| /ATILLA discovery | PASS: VERIFY.BIN (4096 bytes), SETT01.TST (32 bytes); complete=YES |
| Bounded CRC read #1 | PASS: 4096 bytes / 16 chunks / CRC 5ACE03B3 |
| Bounded CRC read #2 | PASS: 4096 bytes / 16 chunks / CRC 5ACE03B3 |
| Reusable filename-restricted reader | PASS: 4096 bytes / 16 chunks / expected CRC 5ACE03B3 |
| Combined outcome | **COMBINED READ-ONLY PASS discovery=PASS crc=PASS asset=PASS NO WRITES** |
| Worker stack | 17,256 bytes minimum free of 24,576 |
| Weather current + forecast | PASS: HTTP 200 / 542 and 1126 bytes |
| CARTO basemap | PASS: six HTTPS and LVGL tile displays |
| RainViewer radar | PASS: six full PNG bodies and decoded/filtered LVGL tiles |
| Loaded free memory | ~4,895 kB PSRAM / ~100 kB internal heap |

## Release advisories

- The three file passes were against the same existing 4KiB file. The observed CRC is useful for detecting accidental change but is **not an independently provisioned trusted asset hash**, and CRC32 is not a cryptographic authenticity check.
- No filesystem writes were attempted by the read-only probe. Older independently gated experimental write test code still exists in the component; do not enable those modes.
- The file reader does not render content, load a manifest, stream an asset into LVGL, or provide concurrent SD access.
- An I/O timeout is cooperative and cannot preempt a blocked FatFs call. SD removal, oversized directories, repeat refresh, intermittent SPI faults, long soak and all touchscreen paths remain untested.
- Image processing produced interval warnings of roughly 176–391 ms. No unexpected reboot is seen in the excerpt.
- Keep 4.7.9w and 4.7.9v as earlier private rollback versions. The complete firmware remains private and is not committed to the public repository.
