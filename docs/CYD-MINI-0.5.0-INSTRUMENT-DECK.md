# ATILLA Mini v0.5.0 — six-screen instrument deck (card-independent)

This checkpoint extends the hardware-tested CYD display and touch configuration. The complete local firmware with private connection settings is distributed separately, **not committed**.

## What changed
- Six pages: HOME / WX / DATA / MAP / SYS / SD.
- GPIO34 ambient light voltage telemetry via ESPHome ADC (15-second reads). This is **raw voltage**, not lux; direction and calibration need hardware verification.
- On-screen SD "Asset Vault" clearly says DRIVER NOT ENABLED. This release does not mount, scan, read, format, or write a card. Device runs without a card.
- NAV continues tap-anywhere cycling while touch transform is not precisely calibrated.
- Same mipi_spi ST7789V, GPIO and XPT2046 setup; encrypted HA API and clock retained.
- Weather comes from configured HA weather entity, not yet matched to Waveshare Open-Meteo data.
- MAP is a visualization concept, **not live radar**.

## Architecture for upcoming SD implementation
ESP32 hardware SPI controllers are constrained. Current display is SPI bus A (GPIO14/13), touch is bus B (GPIO25/32/39), and the likely SD slot uses GPIO18/23/19/5. Three independently instantiated hardware buses aren't available; adding a third via ESPHome auto selection may force slow software SPI. Preferred next implementation is an explicit SD driver with careful transaction/host management and bounded reads, validating display and touch after mount.

Source: https://esphome.io/components/spi/
ADC: https://esphome.io/components/sensor/adc/
MIPI SPI: https://esphome.io/components/display/mipi_spi/

## Next integration
Once FAT32 card is available, mount read-only, query card/filesystem metadata and read a tiny versioned MANIFEST.TXT in bounded chunks before attempting graphics or radar. Storage errors should not prevent HA/UI operation.

No Waveshare YAML files altered. No private API keys, Wi-Fi secrets, or location in this documentation.
