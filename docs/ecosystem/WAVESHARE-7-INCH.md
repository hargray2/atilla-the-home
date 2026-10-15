# ATILLA Waveshare 7-inch — full-size home dashboard

An ESPHome + LVGL weather and smart-home touchscreen for the **Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2** (800 × 480 RGB, GT911 touch, PSRAM and microSD).

The private complete device configuration integrates Home Assistant entities, weather and forecasts, CARTO map tiles, RainViewer radar overlays, and custom SD/FatFs routines. The full `waveshare.yaml` is **not published**. This repository instead contains reusable `components/atilla_storage` and `components/atilla_radar_filter`, plus tests and technical documentation.

## Documented hardware checkpoints

- [4.7.9aa](../VALIDATED-BASELINE-4.7.9aa-WRITE-FIRST-PASS.md): one explicitly enabled immutable 70-byte PNG cache write was staged, verified by CRC and observed intact on a subsequent boot. This is **not** general write access, an HTTP tile cache, or proof of power-loss safety.
- [4.7.9z](../VALIDATED-BASELINE-4.7.9z.md): read-only manifest and bounded asset reader tests, weather and six CARTO/radar tiles observed on hardware.
- [4.7.9y](../VALIDATED-BASELINE-4.7.9y.md): combined read-only discovery and CRC / asset reader tests, an earlier acceptance baseline.

Main-loop display delays, card-recovery edge cases, general SD asset presentation, and long-duration stability still require explicit acceptance evidence. Experimental SD branches hold prototypes and should not be merged or deployed solely because they compile.

Read the [component inventory](../COMPONENT-INVENTORY.md), [architecture](../CURRENT-ARCHITECTURE.md), [hardware](../HARDWARE.md) and [release notes](../RELEASE-NOTES.md). Never flash the generic `tests/*.yaml` fixtures to the actual touchscreen.

[Back to ecosystem](README.md).
