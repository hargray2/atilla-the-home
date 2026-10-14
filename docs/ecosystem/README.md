# ATILLA — ESP development ecosystem

ATILLA is a growing ecosystem for developing, testing and operating ESP-based smart-home devices. There are **three current projects**, organized below by role rather than hardware size.

1. **[ATILLA-LAB — automated development server](ATILLA-LAB.md).** A general-purpose Debian development environment for ESPHome projects, local builds, registered-device flashing safeguards, hardware diagnostics and Home Assistant dashboard generation. Also intended for independent Tuya MCU and smart-appliance conversions. Automation and hardware-write safeguards have different completion levels; see project status.
2. **[ATILLA Waveshare 7-inch](WAVESHARE-7-INCH.md).** ESP32-S3 touchscreen weather, maps and radar dashboard with private full firmware and public reusable components.
3. **[ATILLA Mini — CYD (Cheap Yellow Display)](ATILLA-MINI.md).** Independent compact ESP32 touchscreen firmware, hardware tests and screen-idle behavior.

## Project boundaries

The repo currently primarily stores display firmware components, ATILLA Mini experiments and engineering reports. **ATILLA-LAB's server source resides in a separate checkout**, not here; offline-preview development must not be mistaken for deployed server capabilities.

See [integration and release boundaries](INTEGRATION.md). Each component keeps independent hardware identity, build evidence, deployment authorization, acceptance history and rollback. No device flash or erase authorization is implied by a successful build.

Never commit secrets, private complete Waveshare firmware, backup images or private coordinates. Historical records remain in `docs/` to preserve engineering provenance.
