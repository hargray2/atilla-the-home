# ATILLA Mini — CYD bring-up

Device: user-owned small ESP32 touchscreen photographed front/back; rear PCB marked ESP32-2432S028-series, ESP32-WROOM-32 module, microSD slot, USB-C and Micro-USB. Factory demo boots.

**Do not assume every CYD revision uses identical wiring.** Before flashing, confirm exact silkscreen suffix, controller and touch bus from factory demo source or a clear board image; back up the factory firmware where feasible.

## Independent release track

- Waveshare 7-inch stays at the separately hardware-validated 4.7.9y checkpoint; no modifications to its complete private YAML.
- ATILLA Mini starts with a separate ESPHome config and version namespace once pinout is confirmed.
- Initial gate: display + backlight + touch, Wi-Fi + encrypted API, clock, Home Assistant state; do not mix in SD, PNG decode, radar or complex HTTP graphics before memory checks.
- Later: current weather, compact forecast, modest static map; radar only after heap and refresh profiling.

## Candidate hardware assumptions to verify

- 320x240 SPI TFT, often ILI9341, but some CYD variants have ST7789.
- Common resistive touch chip is XPT2046; exact CS/IRQ/calibration differs by revision.
- ESP32-WROOM-32 often has no PSRAM. Treat available internal heap and largest allocatable block as the governing constraints, not the Waveshare's 8 MB PSRAM metrics.
- SD slot must be verified separately (bus sharing and touch/display chip selects can conflict).

## First test acceptance

On confirmed pinout: boot with one supported display driver, keep touch readable and Home Assistant API reachable; collect 2-minute startup log, available heap, largest block, Wi-Fi stability, and correct orientation. Only then add application pages.

No private credentials, actual complete firmware YAML or precise location in the public repository.
