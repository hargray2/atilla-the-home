# ATILLA Mini NEO 1.5.2 — SD compile fix

The user's ESPHome 2026.9.1 / ESP-IDF 5.5.5 compilation of 1.5.1 reached `main.cpp` before failing at YAML line 124: `dev.transfer_byte(...)` attempted member access on `esphome::spi_device::SPIDeviceComponent*`.

**Fix in released 1.5.2 downloadable YAML:** use `auto *dev = id(mini_sd_wire);` and `dev->transfer_byte(value)`; call `dev->enable()` before SD probe byte transfers and `dev->disable()` after, leaving GPIO5 CS manually controlled for idle clocks; explicitly deassert CS on boot. Remove unused misleading `deselect` lambda. Preserve all UI, real weather, SPI buses, pins, read-only status checking, and ESP-IDF flags.

Both configured (private) and `!secret`-referencing (public) files are available as artifacts in the conversation, not committed to GitHub. Static YAML parsing and source checks pass. **Full ESPHome compile not performed** because CLI/build env not available locally; SD card still requires physical test. No commands to write/format the card. No Waveshare changes.
