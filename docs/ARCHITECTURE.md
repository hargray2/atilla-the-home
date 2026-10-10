# ATILLA architecture — hardware-tested and experimental paths

## Hardware and UI

- Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2; 800×480 LVGL dashboard.
- Display stability testing used RGB pixel clock 14 MHz.
- Six 256×256 CARTO basemap tiles and six matching RainViewer radar tiles (zoom 6, historic x 16–18/y 25–26 arrangement).
- Standard on-screen images use ESPHome `online_image`, decoded into RAM, then presented via LVGL on the ESPHome execution path.

## Main-loop limitation (measured)

`online_image.set_url` performs synchronous network initialization. Radar dispatch averaged about 3.3 s per tile in 4.7.4 instrumentation, followed by independent PNG decode delays. Staggering dispatches reduces overlap but does **not** remove individual stalls. All six displayed tiles still use this original method in 4.7.8d.

## Worker development sequence (private single-file YAML)

1. **4.7.5c — worker HTTPS probe:** FreeRTOS task completed HTTP 200; request ~3,892 ms, no corresponding multi-second main-loop warning (one 272 ms warning).
2. **4.7.6 — full PNG:** worker downloaded a complete ~25 KiB PNG into a bounded 128 KiB PSRAM buffer, checked signature/end and byte count, and released memory.
3. **4.7.7 — ownership handoff:** one-message FreeRTOS queue transferred ownership of a full 256×256 PNG to the ESPHome main loop; consumer verified metadata and released buffer.
4. **4.7.8a–c — main-loop decoder tests:** compilation issue corrected in b; PNGLE allocation failure isolated in c. Available internal heap ~99 KiB but largest internal block ~31 KiB; contiguous PSRAM ~4.5 MiB.
5. **4.7.8d — pending:** explicitly allocate PNGLE decoder state in PSRAM, decode into temporary RGB565 PSRAM frame, free diagnostics. No hardware result yet.

The worker must **never** call LVGL or ESPHome `online_image` from its own thread. Queue-owned buffers require bounded length, a single owner, timeout/error cleanup and main-loop-only rendering.

## Storage

The public `main` branch deliberately provides **read-only** raw SD transport. Advanced FatFs and persistence work exists on experimental branches and was invoked by private YAML. Do not confuse a hardware-tested experimental branch with promotion of that source to `main`.

The latest settings-persistence logs repeatedly report `SETTINGS PERSISTENCE PASS (existing record across boot; read-only)`. This validates a small test record, **not** a production settings storage service, atomic updates, SD asset cache or power-failure tolerance.

## Private firmware and source control

The full `waveshare.yaml` remains outside this public repository. Documentation here summarizes test outcomes without secrets or live map URLs. Pin component branches/commits deliberately; maintain tested rollback versions.
