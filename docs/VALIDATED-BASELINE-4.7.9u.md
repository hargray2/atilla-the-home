# ATILLA 4.7.9u — hardware acceptance

**Date:** 2026-10-11. **Target:** Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2, ESPHome 2026.9.1.

## Scope

4.7.9u moves the two formerly separate external component references to a **single immutable Git source commit** `5581459f87753977151f43f7a853f3604efa1b84`. The imported source for `atilla_storage` and `atilla_radar_filter` was confirmed byte-for-byte identical to the prior branch sources. No UI or runtime operation change was intended.

## Boot and runtime log evidence

| Gate | Result | Observation |
|---|---|---|
| Firmware startup | PASS | Compiled 2026-10-11 01:05:11 -0400, ESPHome 2026.9.1 |
| Weather current | PASS | kind=0, HTTP 200, 542 bytes, 1,622 ms |
| Weather forecast | PASS | kind=1, HTTP 200, 1,130 bytes, 3,159 ms |
| Weather publish | PASS | LIVE / UPDATED, current sensor states updated |
| CARTO | PASS | Six HTTPS downloads and six LVGL display successes |
| RainViewer | PASS | Six full PNG body reads, PSRAM decodes/filter/display successes |
| FatFs | PASS | SDHC/FAT32 init; f_mount=0; read/CRC; root directory; f_unmount=0 |
| Known file CRC | PASS | 12 bytes CRC32 630105D1 |
| SD worker stack | PASS | 24,576-byte task, minimum 17,388 bytes free |
| PSRAM after loading | OBSERVED | 4,895 kB free |
| Internal heap after loading | OBSERVED | ~100 kB free |
| Ambient display | PASS | Activated at approximately 2 minutes |

## Non-blocking observations

- Initial ESPHome API connection closed with EOF; next connection and encrypted handshake succeeded. Root cause not established by this log.
- Rendering and scheduling intervals reached 181–493 ms; known image-main-loop costs persist.
- Largest internal heap free block observed at 31 kB for much of the initial run and later 53 kB. This metric varies with allocation lifetime.
- No restart seen in the provided approximately two-minute post-boot excerpt. This is **not** a long-duration soak or a full screen/control interaction test.

## Release decision

**4.7.9u is hardware-validated for early startup and core weather/radar/CARTO/read-only-storage operation.** It supersedes 4.7.9t as the latest initial-hardware acceptance checkpoint, while 4.7.9t remains a known-good rollback.

The repository overhaul PR can be reviewed, but its merge and any future SD asset-loader deployment remain separate release decisions. Existing FatFs functionality is read only; no writes were attempted.
