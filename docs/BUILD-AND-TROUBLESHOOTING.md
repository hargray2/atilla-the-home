# ESPHome build and troubleshooting

## Current checkpoint

The private complete `waveshare.yaml` for ATILLA 4.7.9t compiled and passed early hardware runtime checks using **ESPHome 2026.9.1**. It is not checked into GitHub. Both the storage and radar filter custom components are present in the overhaul branch; isolated CI does not build the private dashboard.

Current external component source commits and verification scope are recorded in [the lock manifest](../release/components-4.7.9t.lock.json).

## Installation rules

The device is managed from iPhone through Home Assistant ESPHome Builder. Preserve the **complete working private YAML** as rollback and never flash `tests/fatfs-smoke.yaml` or `tests/radar-filter-smoke.yaml` onto the panel (their generic GPIO mappings are CI-only, not Waveshare CH422G wiring).

Changing an external component Git branch or commit can affect validation and compilation. `refresh: 0s` requests a cache refresh but is not an immutable version pin. For a release, record the full commit SHA and the complete private YAML, not just its version label.

## Historical resolved errors

- `spi_device.SPIDeviceComponent` AttributeError: Python integration uses `spi_device.spi_device`.
- `atilla_storage` ambiguous namespace: fixed by consolidating namespace declarations.
- FatFs test flags: require `fatfs_mount_test: true` plus **at most one** read probe mode.
- 4.7.9p storage main-loop overrun/stack warning: 4.7.9q's dedicated 24 KB worker passed.
- Weather 4.7.9r TLS verification error: 4.7.9s serialized connections.
- Weather 4.7.9s HTTP 200 bodies marked failed: 4.7.9t fixed unknown Content-Length handling; workers PASS.

## Diagnostics for future failures

Separate failures by phase: YAML parser, ESPHome Validate, component compile, OTA flash, and runtime test. Record the **first meaningful compiler error**, not the last Ninja failure line. Confirm component commit IDs, logs and actual hardware behavior. Redact API keys, Wi-Fi credentials, secrets, private coordinates and query tokens from any public report.

## CI limitations

Storage and radar smoke tests are isolated compile fixtures. Passing them does not prove complete device YAML compilation, full LVGL page function, actual CH422G SD wiring or runtime multi-day stability. See [automated tests](AUTOMATED-TESTS.md).
