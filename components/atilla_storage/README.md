# ATILLA 4.6 storage component — experimental

The first **read-only SD SPI transport** is in [`atilla_storage.h`](./atilla_storage.h). It wraps the two ESPHome SPI devices used by ATILLA (clock-only SPI, and SD with CH422G EXIO4 CS) and supports SDHC initialization and CMD17 physical-sector reads.

**Not flash-ready.** The component currently has no tested runtime integration; `__init__.py` is a placeholder. No FATFS mount is implemented. There is no file creation, writing, radar asset caching or OTA firmware here. The C++ source has **not** been built in ESPHome 2026.9.1. Keep known-good ATILLA 4.5 running.

Hardware: SCK GPIO12, MOSI GPIO11, MISO GPIO13; SD CS on CH422G EXIO4.

Next engineering milestones: register component with ESPHome, validate compilation, run nonblocking read-only tests, then implement disk-I/O adapter + FATFS mount without formatting. A properly verified write driver must come later.

Only reusable public driver code belongs here—do **not** upload the private dashboard YAML, Wi-Fi password, API encryption key, or map tokens.
