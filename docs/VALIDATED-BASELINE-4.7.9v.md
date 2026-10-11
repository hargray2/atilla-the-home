# ATILLA 4.7.9v — read-only SD asset discovery acceptance

Date: 2026-10-11. Device: Waveshare ESP32-S3-Touch-LCD-7 rev 1.2, ESPHome 2026.9.1. Firmware build timestamp: 2026-10-11 01:24:08 -0400.

## Hardware evidence

| Gate | Result | Observation |
|---|---|---|
| Compile, boot and encrypted API | PASS | API handshake succeeds; firmware identifies ESPHome 2026.9.1 |
| SDHC/FAT32 and FatFs mount | PASS | Read-only initialization; f_mount=0, ~140 ms |
| New /ATILLA discovery | PASS | 2 files, complete=YES, readdir_rc=0, close_rc=0 |
| Entries | OBSERVED | VERIFY.BIN (4096 bytes); SETT01.TST (32 bytes) |
| FatFs unmount | PASS | f_unmount=0 |
| SD worker stack | PASS | 24,576-byte worker; minimum 17,512 bytes free |
| SD writes | NOT ATTEMPTED | Explicit NO WRITES; no formatting, asset file reads or writes |
| Weather | PASS | Current kind=0 HTTP 200 541 bytes; forecast kind=1 HTTP 200 1129 bytes; LIVE / UPDATED |
| CARTO | PASS | Six HTTPS tile downloads and six displays |
| RainViewer | PASS | Metadata and six PNG tile decode/filter/display passes |
| Post-load memory | OBSERVED STABLE | 4,895 kB PSRAM and ~100 kB internal heap free |
| Ambient UI | PASS | Activated around 2 minutes |

## Advisories

- Main-loop performance warnings were observed: CH422G operation 1,484 ms; LVGL operation 1,121 ms; Wi-Fi signal sensor 628 ms; and several intervals up to ~497 ms. Root causes were not isolated. This checkpoint is a **functional startup pass**, not a latency or prolonged-stability qualification.
- The existing raw known-file 12-byte CRC check passed; the new discovery probe **does not read the contents** of discovered files. No manifest parsing or integrity verification of these assets is implemented.
- Directory enumeration is deliberately limited to 32 immediate entries. This card contained only 2. Handling a directory with more than 32 entries and card removal under load has not been hardware-tested.
- The private complete firmware remains outside the public repository. Test fixtures use dummy GPIO mappings and must not be flashed to this panel.
- Keep 4.7.9u as the previous hardware-tested rollback and preserve the pinned component commit.

## Next gate

A separate opt-in, bounded file-content read/CRC mode using the existing SD worker, with explicit size, chunk and timeout limits, followed by a full private firmware compile and hardware log acceptance.
