# ATILLA architecture — hardware checkpoint 4.7.9t

## Boundaries

- **Private firmware `waveshare.yaml`:** single distributable full device configuration including LVGL layouts, sensor IDs, controls, clock, weather, CARTO and RainViewer logic. Not in public repository.
- **`components/atilla_storage`:** custom SD protocol/FatFs read-only diagnostics; hardware-tested via dedicated task from `dev/sd-batch-worker-479q`.
- **`components/atilla_radar_filter` on `dev/radar-intensity`:** precipitation filter used by the private firmware.
- **Current repository `main`:** older, conservative branch; not representative of the live 4.7.9t firmware.

## Runtime ownership

| Execution context | Responsibility | Hardware evidence |
|---|---|---|
| ESPHome main loop | UI/LVGL mutations, publishing sensor values, handoff processing | Weather and radar published/displayed |
| Weather FreeRTOS workers | Secure, serialized Open-Meteo HTTPS GETs | kind=0 and kind=1 PASS; weather feed LIVE / UPDATED |
| CARTO worker | Six dark basemap HTTPS downloads | 6/6 HTTP 200; LVGL display PASS |
| Radar metadata + tile workers | Metadata polling and six sequential PNG tile downloads | Six full-body PNG PASS and display PASS |
| SD FreeRTOS worker | Read-only SDHC/FAT32 init, FatFs mount, known-file CRC and directory traversal | FATFS BATCH READ-ONLY PASS, stack minimum-free 17,344 bytes |

Image raster data lives in PSRAM, with LVGL updated only after worker handoff. The current six CARTO basemap images are RGB565; the six radar overlays use ARGB8888 and are filtered to hide precipitation below 30 dBZ. Some PNG decode and image-display intervals still generate ~180–385 ms watchdog-style component performance warnings; this is not the earlier multi-second weather networking stall.

## Known limitations

- Full private YAML cannot currently be compiled by public CI.
- The SD path is **read-only diagnostic**, not a general asset service. It opens a known 12-byte file and enumerates the root; it has not established large asset stream/read support or durable writes.
- Current live software still depends on moving GitHub component branches.
- Some older docs describe 4.5/4.6 milestones; prefer this document and the acceptance matrix for current status.
