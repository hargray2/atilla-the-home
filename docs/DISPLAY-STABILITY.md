# ATILLA display stability — brief horizontal blips

## Hardware and symptom

Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2, 800×480 RGB panel driven by ESPHome/LVGL. User reports **very fast horizontal lines / tearing that disappear immediately**, occurring **randomly even when idle**. No photograph or oscilloscope trace yet. The fact that the blips are transient does not by itself prove a hardware or software root cause.

## Ranked hypotheses

1. RGB framebuffer scan-out contention / non-synchronized LVGL rendering (tearing).
2. RGB DMA underflow or PSRAM bandwidth contention (map/radar tiles, image decoding, Wi-Fi or SD I/O).
3. Marginal LCD pixel clock or RGB porch/timing configuration.
4. Electrical signal integrity, display ribbon connection, or 5V/3.3V power problems.

Image loading and SD-card loop blocking may exacerbate timing, but idle symptoms mean they cannot be assumed to be the sole triggers.

## Important distinction

ESP-IDF `CONFIG_LCD_RGB_RESTART_IN_VSYNC` is designed to recover a shifted RGB DMA stream at VSYNC. It is **not a universal anti-tearing switch**. Only enable if the exact display driver and sdkconfig support it and there is evidence of shift/desync. Reducing PCLK slightly is a controlled alternate test *only* after measuring the current `pclk_frequency` and documented panel tolerances. Double-buffering changes PSRAM footprint and must be evaluated against dashboard plus 12 PNG tiles.

## Safest isolation plan

1. Preserve the current known-running firmware; don't combine display changes with experimental FATFS `f_open` testing.
2. Obtain the actual private `waveshare.yaml` display block. Identify `rpi_dpi_rgb`, `esp32_i80` or external RGB driver, existing pixel clock, update interval, auto-clear, LVGL render cadence, framebuffer mode, buffer sizes, and SDK options. Do not infer these from board name.
3. First make an **observability-only** revision or one minimal display change. Verify ESPHome 2026.9.1 compilation and hardware support before OTA.
4. Monitor idle blips for a comparable period (at least 20–30 minutes), then HOME and RADAR independently, taking note of number and duration.
5. If one setting helps, test with full radar preload. Retain rollback firmware. If not, restore original setting before trying a different variable.
6. If no software change helps, check cable seating, clean stable power source and backlight signal without altering SD or Wi-Fi secrets.

## Security and scope

Do not commit private YAML, home coordinates, SSID, passwords or API keys. This branch documents display experiments; FATFS remains on its separate `dev/fatfs-readonly` branch. No display modification has yet been built or flashed.
