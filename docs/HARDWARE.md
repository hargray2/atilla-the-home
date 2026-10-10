# ATILLA hardware reference

## Target board

- Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2
- 800×480 RGB display with LVGL and GT911 touch
- ESP32-S3 with approximately 8 MiB PSRAM reported in diagnostics
- CH422G I/O expander on I²C GPIO8 (SDA), GPIO9 (SCL)
- LCD backlight GPIO16 PWM; CH422G EXIO2 also participates in backlight power control

## SD card interface (observed and tested)

| Signal | Connection |
|---|---|
| MOSI | GPIO11 |
| CLK | GPIO12 |
| MISO | GPIO13 |
| SD chip select | CH422G EXIO4, **not a native MCU GPIO** |

Card used in tests: Samsung EVO U3 64GB microSD, FAT32.

Important: a generic ESP-IDF SDSPI host normally expects a GPIO CS. A solution must explicitly cooperate with the CH422G expander or route through an existing ESPHome SPI device that controls the expander.

## Runtime constraints

From historical ATILLA 4.4–4.5 logs:
- Before heavy image loads: internal free ~103,392 B; largest contiguous internal block 31,744 B; PSRAM free ~7,372,096 B.
- After all 12 map/radar images: around 4.65 MiB PSRAM remained; internal largest contiguous block remained ~31 KiB.
- Raw SD diagnostic may block the ESPHome component loop for ~330–343 ms; network downloads have occasionally incurred ~3-second blocking warnings.

These are specific historical observations, not a general performance guarantee.

## Unknowns

Pin assignments, electrical characteristics and APIs beyond this list must be confirmed against this exact Waveshare revision and the installed ESPHome toolchain. Do not assume other 7-inch Waveshare products use the same wiring.
