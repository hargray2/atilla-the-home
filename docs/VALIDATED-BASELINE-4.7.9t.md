# Acceptance record — ATILLA 4.7.9t

Observed device: ESPHome 2026.9.1, ESP32-S3, 8 MB PSRAM, Waveshare 7-inch 800 × 480 display. Source: ESPHome API runtime logs provided on 2026-10-11.

| Gate | Status | Runtime observation |
|---|---|---|
| Device flash/start | PASS | Compiled 2026-10-11 00:33:19 -0400; startup and API connection |
| Weather current | PASS | WEATHER WORKER PASS kind=0, HTTP 200, 542 bytes, 1607 ms |
| Weather forecast | PASS | WEATHER WORKER PASS kind=1, HTTP 200, 1128 bytes, 3149 ms |
| Weather display state | PASS | LIVE / UPDATED; temperature, feels-like, humidity, wind, conditions published |
| CARTO | PASS | Six MAP WORKER HTTPS PASS and six MAP LVGL DISPLAY PASS |
| Radar | PASS | Six PNG FULL BODY PASS and six WORKER TILE DISPLAY PASS |
| microSD | PASS | SDHC/FAT32 read-only init, FATFS f_mount=0, known 12-byte file CRC32 630105D1 |
| FatFs read-only batch | PASS | open/read/CRC/root directory; FATFS BATCH READ-ONLY PASS |
| SD stack | PASS | 24,576-byte task allocation, 17,344-byte minimum free recorded |
| Memory at idle | OBSERVED STABLE | ~4,895 kB PSRAM free, ~100 kB internal heap, ~49 kB largest block |
| Full SD asset loading | NOT TESTED | Only a 12-byte known file and root listing tested |
| Durable SD writes | NOT ENABLED | No writes attempted in this checkpoint |
| Long-duration soak | NOT TESTED | Runtime excerpt covers initial boot and early operation |

The weather TLS failure from 4.7.9r and the response length rejection in 4.7.9s were resolved by 4.7.9t. Logs do not establish multi-day stability or verify the visual accuracy of every map image independently.

**Rollback:** Retain the complete original private 4.7.9t TXT/YAML and matching external-component revision before any future integration.
