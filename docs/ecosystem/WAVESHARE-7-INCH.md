# ATILLA Waveshare 7-inch — ESP32-S3 home dashboard

**Hardware:** Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2, 800 × 480 RGB display, GT911 touch, microSD and PSRAM. [Hardware reference](../HARDWARE.md).

**Role:** the full-size touchscreen home dashboard with weather and forecasts, CARTO basemap images and RainViewer radar overlays. It is separate from ATILLA-Mini and the Debian engineering station.

## Firmware provenance and branch status

The complete authoritative `waveshare.yaml` is **private and deliberately absent** from this public repository. Public source here includes `components/atilla_storage` and `components/atilla_radar_filter`, plus partial test fixtures; a passing fixture cannot prove that the private full dashboard builds or works on real hardware.

- **On this documentation branch:** [4.7.9y hardware record](../VALIDATED-BASELINE-4.7.9y.md) is the latest Waveshare acceptance report carried in its ancestry. It confirmed weather, six CARTO/radar tiles, FatFs read-only discovery, repeated CRC and bounded asset-reader operations.
- **On `main` as inspected 2026-10-15:** newer [4.7.9z manifest read-only acceptance](https://github.com/hargray2/atilla-the-home/blob/main/docs/VALIDATED-BASELINE-4.7.9z.md) and [4.7.9aa opt-in immutable-cache write-pilot acceptance](https://github.com/hargray2/atilla-the-home/blob/main/docs/VALIDATED-BASELINE-4.7.9aa-WRITE-FIRST-PASS.md) exist. These source branches **have diverged**. The 4.7.9aa record covers one 70-byte PNG write-and-verify and a separate reboot-persistence test; it does **not** establish a general SD write service, JPEG/HTTPS graphics caching, power-loss safety or SD-to-LVGL rendering.

Do not silently merge the different storage implementations or move mutable branch references into a live private device configuration. Reconcile code and hardware evidence before any firmware promotion.

## Engineering rules

- Keep LVGL/UI updates on the ESPHome main loop; expensive HTTPS/storage operations belong on bounded workers.
- Preserve the known-good full private YAML and SD backup before any write-capable experiment.
- Keep SD writes and formatting disabled unless specifically planned, authorized, backed up and tested for the board.
- Treat source inspection, config validation, compile, OTA update and physical display acceptance as **separate evidence levels**.
- Map/radar tiles being downloaded and decoded is not by itself proof that every refresh or touchscreen interaction is stable.

**More information:** [component inventory](../COMPONENT-INVENTORY.md) · [release notes](../RELEASE-NOTES.md) · [storage validation](../SD-VALIDATION.md) · [integration boundary](INTEGRATION.md).
