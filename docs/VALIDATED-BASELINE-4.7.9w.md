# ATILLA 4.7.9w — bounded file CRC acceptance

Date: 2026-10-11. ESPHome 2026.9.1 compiled at 01:39:23 -0400. Hardware: Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2.

## Verified from physical-device log
| Gate | Result | Evidence |
|---|---|---|
| Boot and encrypted API | PASS | API handshake; chip rev 0.2 |
| FAT32 / FatFs | PASS | SDHC init; f_mount=0 (145ms); f_unmount=0 |
| Known 12-byte file | PASS | WPSETT~1.DAT CRC32 630105D1 |
| 4KiB bounded read pass 1 | PASS | VERIFY.BIN 4096 bytes; CRC32 5ACE03B3; 16 chunks |
| 4KiB bounded read pass 2 | PASS | VERIFY.BIN 4096 bytes; CRC32 5ACE03B3; 16 chunks |
| Repeatable checksum | PASS | Both CRC values equal; read-only; no writes |
| Storage worker stack | PASS | Minimum free 17,272 bytes in 24,576-byte worker |
| Current and forecast weather | PASS | HTTP 200; 542 and 1125 bytes; LIVE / UPDATED |
| CARTO | PASS | All six tiles downloaded and displayed |
| RainViewer | PASS | Six PNG bodies, decode/filter/display successes |
| Memory after preload | OBSERVED | 4,895 kB PSRAM; 100 kB internal heap free |
| Ambient display | PASS | Activated after ~2 minutes |

## Advisories
- This is **hardware-validated startup behavior**, not a long soak, periodic radar refresh acceptance, unplug test, or full touchscreen regression suite.
- CRC `5ACE03B3` was observed twice on the same card and file, establishing read repeatability, **not authenticity** against a separately trusted reference.
- The code limits individual read requests to 256 bytes, total size to 4096 bytes, and checks 10 seconds between blocking reads; the time guard **cannot interrupt** a blocked FatFs I/O call.
- Remaining main-loop warnings observed included image interval ~176–391 ms and API ~400 ms. These do not appear linked to the SD probe but their root causes remain unproven.
- No file writes/formatting were attempted by the storage probe. The component still includes separately gated historical experimental write test modes; do not enable those in production.
- Use 4.7.9v as the previous rollback and keep a full private copy of 4.7.9w.

The complete private firmware is not present in the public repo; CI independently compiled the storage and combined-component fixtures.
