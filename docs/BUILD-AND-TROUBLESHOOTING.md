# ESPHome build and troubleshooting

## Installation workflow

ATILLA is managed via iPhone and Home Assistant ESPHome Builder. The production configuration is one large `waveshare.yaml`; GitHub stores additional component source retrieved at **build time**. Do not ask the user to manually copy C++ headers to the ESPHome filesystem.

A representative external-component section is:

```yaml
external_components:
  - source: github://hargray2/atilla-the-home@main
    refresh: 0s
    components: [atilla_storage]
```

`refresh: 0s` requests a fresh Git repository check for every validation/build. Once releases stabilize, pin to a tested Git tag/commit rather than mutable `main`.

## Failure history

### Validation: AttributeError, SPI device class

```text
AttributeError: module 'esphome.components.spi_device'
has no attribute 'SPIDeviceComponent'
```

Cause: Python config schema referenced `spi_device.SPIDeviceComponent`, while ESPHome 2026.9.1 defines the codegen type as `spi_device.spi_device`. Fixed for both `card_device` and `idle_device` on GitHub.

### Clean Build Files failed with the same AttributeError

ESPHome must first load the configuration (including cached external Python modules), so a broken cached module may prevent the Clean command from proceeding. ESPHome 2026.9.1 external components default to a one-day refresh. Set `refresh: 0s` and validate again to retrieve the fix. An unchanged error may mean a stale repository cache or a different ref; confirm the fetched file contents.

### C++ compile: ambiguous `atilla_storage` namespace

```text
error: reference to 'atilla_storage' is ambiguous
```

The original header declared a global `namespace atilla_storage` as well as `namespace esphome::atilla_storage`. Generated `main.cpp` encountered both. The header was updated to put the transport in `esphome::atilla_storage` as well. **The user has not yet reported a successful new build after this fix.**

### TLS connection failures during radar preload

Earlier rapid tile requests caused SSL/setup resource errors or HTTP connection failures. Historical stable transfer gaps were ~1.5 seconds (map) and ~4 seconds (radar); do not tighten them casually.

### OTA rollback

A more aggressive FAT32 directory scan in the 4.0.3 experiment was followed by rollback. Conservatively limited sector reads later succeeded. An OTA rollback cannot alone establish the precise root cause.

## Reporting future errors

Record ESPHome version, current Git commit or tag, first meaningful compiler error (not only Ninja's final failure), and whether Validate, Compile, or OTA Install failed. **Redact secrets, API keys, Wi-Fi credentials, access tokens, private IP information if sensitive, and query parameters from map URLs.**
