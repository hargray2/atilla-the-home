# ATILLA ecosystem

ATILLA is an ecosystem of three separately maintained parts:

1. [Waveshare 7-inch](WAVESHARE-7-INCH.md): primary ESP32-S3 touchscreen weather/radar display.
2. [ATILLA Mini](ATILLA-MINI.md): separate compact CYD ESP32 display and firmware.
3. [ATILLA-LAB](ATILLA-LAB.md): Debian development server, also intended for unrelated ESPHome, Tuya MCU and smart-appliance projects.

Each component has a distinct source boundary, build/release process, hardware acceptance evidence and rollback procedure. Compilation is not hardware acceptance, and an offline-tested server preview is not deployed firmware.

Shared rules: keep secrets and firmware backups out of Git; require physical identity and job-scoped authorization for hardware writes; preserve private Waveshare firmware; document known limitations explicitly.

See [integration boundaries](INTEGRATION.md). Historical engineering reports remain in `docs/`.
