# ATILLA Mini NEO 1.5.5 — Touch Response Edition (experimental)

Based on the **device-proven 1.5.4 CYD firmware** (ESP32 rev 3.1, ESPHome 2026.9.1, ESP-IDF 5.5.5). This release attempts to address observed intermittent 2–4-second synchronous Open-Meteo blocking and 276ms LCD operations.

## Changes in both conversation YAML artifacts

- Keep the verified 20MHz ST7789V display SPI rate, the XPT2046 pin/calibration, ESP-IDF chip optimizations, the real Open-Meteo forecast data and the read-only FAT32/card/file CRC probe.
- Touch sampling from 50ms to **25ms**.
- Add **mini_touch_down** touch-state guard; do not schedule full-screen redraw while the finger is held down.
- Set display `update_interval: never`, `auto_clear_enabled: false` (render lambda paints the entire screen), and add a 100ms scheduler. Touch events dirty the screen for redraw after release, rather than waiting for the 1s timer. Noninteractive screens redraw less often: Home/System ~5s, scope 2s, weather/static 30s, arcade 300ms while active, FX ~650ms.
- Replace synchronous `http_request.get` call with **ESP-IDF esp_http_client HTTPS** in one FreeRTOS task on core 0. The task streams up to 4095 response bytes into a checked bounded buffer and sends a pointer through a one-slot FreeRTOS queue. The ESPHome 100ms interval consumes/validates/parses it and updates the existing weather globals. No UI-state mutations occur in the networking task. Maintain certificate verification via `esp_crt_bundle_attach` and the existing configured ESPHome `http_request` component's trusted-CA setup. Worker creation and allocation failures are logged; previous live weather remains visible if a request fails.
- On-board first-bank HOME is a SYNC action on Home, now launches the background weather worker.

## Expected logs

- Successful real Open-Meteo update should log `Async HTTPS ... ms status=200 result=OK` and `Open-Meteo real weather and 3-day forecast updated (background)`.
- The 2–4-second `interval took a long time` warning from synchronous HTTPS should disappear. A shorter interval/display warning can still occur **during a display redraw** (~276ms previously) because the framebuffer operation remains synchronous; faster touch polling alone cannot eliminate that cost.
- SD no-card behavior should remain `No SD idle response (CMD0=FF)` without blocking boot or weather. Inserting the FAT32 card and tapping System should still run a bounded read-only probe.

## Verification and release status

Public/private YAML artifacts: `atilla-mini-neo-1.5.5-touch-responsive-{public,private}.yaml`, downloadable in the conversation. **YAML structure was parsed and static checks were run; full ESPHome C++ compilation and hardware test have NOT been performed.** This is an experimental FreeRTOS/TLS change. Keep 1.5.4 as the working rollback.

The complete **credential-free 1.5.5 YAML** is committed at `experimental/atilla-mini/atilla-mini-1.5.5-touch-responsive.yaml`; the private configured artifact stays downloadable only in the chat. Do not commit the private artifact or its credentials. The 7-inch Waveshare project and SD contents were not modified.
