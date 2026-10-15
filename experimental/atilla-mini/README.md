# ATILLA Mini — CYD touchscreen firmware

**ATILLA Mini** is the compact, independent ESP32 touchscreen member of the ATILLA ecosystem. It is not the Waveshare 7-inch firmware, and its board-specific pinout and hardware acceptance do not apply to other ESP devices.

**Current source in this branch:** `atilla-mini-1.5.7-idle-display.yaml`, a credential-free ESPHome configuration for the ESP32-2432S028 dual-USB CYD. Previous versioned YAML files are preserved as development and regression snapshots—not as interchangeable production firmware.

## Current entry points

- [Mini project overview and hardware evidence](../../docs/ecosystem/ATILLA-MINI.md)
- [Version 1.5.7 hardware acceptance](../../docs/CYD-MINI-1.5.7-HARDWARE-ACCEPTANCE.md)
- [Version 1.5.7 idle-display design](../../docs/CYD-MINI-1.5.7-IDLE-DISPLAY.md)
- [Mini SD/peripheral research](../../docs/CYD-MINI-SD-ROADMAP.md)
- [Repo-wide engineering boundaries](../../docs/ecosystem/INTEGRATION.md)

### Firmware snapshots

| Source | Role |
|---|---|
| `atilla-mini-0.1.0-st7789v.yaml`, `0.1.1-sntp` | Initial display, touch, and clock bring-up candidates |
| `0.2.0-neo`, `0.2.1-mipi-spi` | Early NEO display iterations |
| `0.6.0-instrument-deck` | Instrument-deck experiment |
| `1.5.2-hardware-pass` | Earlier hardware-acceptance candidate |
| `1.5.3-asset-vault-preview`, `1.5.4-known-file-crc` | Read-only storage and asset experiments |
| `1.5.5-touch-responsive`, `1.5.6-touch-latency` | Touch responsiveness tuning |
| **`1.5.7-idle-display`** | Most recent versioned Mini source in this branch; idle/display-protection changes |

All filenames in this table begin with `atilla-mini-` and end in `.yaml`. The versioned filenames are intentional. Do not assume that a generic `atilla-mini.yaml` exists.

## Validate and compile — no flashing

Run from the repository root with ESPHome 2026.9.1 and a **private**, local `experimental/atilla-mini/secrets.yaml` containing the referenced `!secret` keys:

```sh
esphome config experimental/atilla-mini/atilla-mini-1.5.7-idle-display.yaml
esphome compile experimental/atilla-mini/atilla-mini-1.5.7-idle-display.yaml
```

These commands compile locally; they do **not** upload firmware to a device. Never commit `secrets.yaml`, tokens, device backups, private network identifiers, or coordinates.

**ATILLA-LAB status (2026-10-15):** the registered `atilla-mini` target passed remotely queued validation and compilation, with hardware writes disabled. At that time the remote job summary did not expose its exact configuration filename, so that result must not be represented as proof that the repository's 1.5.7 file was the compiled input. Confirm source path and build digest before promoting a release.

## Hardware evidence and unresolved checks

The 1.5.7 acceptance report documents background weather retrieval, idle dimming, backlight-off behavior, Wi-Fi continuity and uptime. It does **not** establish every first-touch wake path, successful inserted-card CRC, long-running weather polling, or arbitrary SD asset rendering. Distinguish a successful config check, local build, OTA upload and observed physical behavior.

The original 0.1.0 first-flash candidate and hardware-recovery notes remain historical context in [Mini bring-up](../../docs/CYD-ATILLA-MINI-BRINGUP.md). Do not use `esphome run`, serial/OTA upload, erase, or reset as part of a routine build. Any hardware write requires the correct board identity, a verified backup, specific authorization and recovery planning.

For 1920 × 1080 camera-assisted acceptance, use ATILLA-LAB's registered camera. Screenshots alone do not prove correct touch or filesystem behavior.
