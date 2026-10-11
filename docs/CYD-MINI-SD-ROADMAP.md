# ATILLA Mini — CYD microSD and peripheral roadmap (research, 2026-10-11)

## Status
- Confirmed on owner's device: display (ST7789V via ESPHome mipi_spi), XPT2046 touch events, Wi-Fi/API and Home Assistant time, touchscreen page changes.
- Not confirmed yet: SD mount, card capacity, SD reading under graphics load, ambient light, RGB LED, audio, touch coordinate mapping for direct buttons.
- Keep separate from Waveshare ESP32-S3/PSRAM firmware. This classic ESP32 should be assumed **no PSRAM** unless actually measured.

## Hardware pin map (candidate for ESP32-2432S028)
| Function | Pin |
|---|---|
| LCD SCLK / MOSI / CS / DC / backlight | GPIO14 / 13 / 15 / 2 / 21 |
| XPT2046 clock / MOSI / MISO / CS / IRQ | GPIO25 / 32 / 39 / 33 / 36 |
| SD SPI SCLK / MOSI / MISO / CS | GPIO18 / 23 / 19 / 5 |
| LDR ADC | GPIO34 |
| RGB LED (active LOW) | GPIO4 / 16 / 17 |
| Amplifier input | GPIO26 |
| I2C extension | GPIO22 / 27 |

Source pin references:
- https://github.com/khanhj/ESP32_2432S028/blob/main/PINS.md
- https://github.com/boxwrench/DIST_ESP32-CYD-Tester/blob/main/docs/ESP32_CYD_REFERENCE.md
- https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/sdmmc.html

## SPI implementation warning
SPI bus pins do not automatically imply three independent hardware SPI controllers. Classic ESP32 has limited available SPI controllers; use explicitly configured bus instances or deliberate SPI transaction switching and separate chip selects. Do not initialize SD through SD_MMC 4-bit mode: this board's slot is documented with SPI pins, not native SDMMC pins.

## Preferred roadmap
1. Single integrated v0.5 firmware: existing NEO UI + quiet SD card mount/metadata health, optionally LDR and RGB indicator. If SD card absent, preserve dashboard operation, clock and HA connectivity.
2. SD read-only asset pack: small indexed RGB565 or palette-encoded icons, backgrounds, precomputed weather graphs. Buffer one small chunk at a time; reject oversized assets. Card removal must not reboot device.
3. Weather source harmonization with Waveshare: use same Open-Meteo location/feed or export 7-inch readings into HA. Weather entity attributes are not guaranteed to equal standalone sensor measurements.
4. HA room controls with measured touch targeting. Keep deliberate tap-to-cycle as fallback until coordinates are calibrated.
5. Radar proof: preprocess small compressed or palette tile frames externally and store frames on SD. Show static frame first; no bulk HTTPS PNG decoding at full image resolution without memory profiling.
6. Cache forecast/offline assets; ambient-light dimming and RGB notification colors; optional discreet audio feedback if GPIO26 amplifier is confirmed.

## SD layout proposal
```
/ATILLA-MINI/
  MANIFEST.TXT
  ICONS/
  WEATHER/
  RADAR/
  MAPS/
  THEMES/
```
Use strictly bounded filenames and size checks. SD should be optional and treated as untrusted removable media. Prefer read-only mounting for early releases; avoid full-card directory scans during LCD drawing.

## Security
Do not add Wi-Fi passwords, ESPHome API encryption keys, precise location, or private full YAML to this repo. Private ready-to-flash copies belong outside public Git history.

## Feasibility references
- ESP-IDF SDSPI/FAT mounting: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/sdmmc.html
- ESPHome ESP32 filesystem build options: https://esphome.io/components/esp32/
- Arduino library considerations for SD/FS in ESPHome: https://developers.esphome.io/blog/2026/02/12/esp32-arduino-selective-compilation-libraries-disabled-by-default/
