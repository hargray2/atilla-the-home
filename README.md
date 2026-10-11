# ATILLA THE HOME

ESPHome + LVGL dashboard for the **Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2** (800 × 480, GT911 touch, 8 MB PSRAM, microSD).

## Current project checkpoint

**ATILLA 4.7.9u — hardware-validated startup and core systems 2026-10-11.** The device logs confirm secure background current-weather and forecast HTTPS requests, six CARTO map tile downloads/displays, six RainViewer radar tile downloads/decodes/displays, and read-only FatFs operations on a dedicated worker with a 24,576-byte stack. At idle after the initial imagery loads, approximately 4,895 kB PSRAM and 100 kB internal heap remained free in the observed log window.

This public repository contains **custom external components, tests, and engineering documentation**. The authoritative complete firmware is a **private, single `waveshare.yaml`**, supplied as a complete .txt/YAML file for ESPHome Builder. **Do not commit the complete private YAML or API keys here.** `main` is not the current hardware-validated firmware baseline.

## Start here

- [Repository structure and component responsibilities](docs/CURRENT-ARCHITECTURE.md)
- [4.7.9u hardware acceptance](docs/VALIDATED-BASELINE-4.7.9u.md)
- [Previous 4.7.9t hardware acceptance](docs/VALIDATED-BASELINE-4.7.9t.md)
- [Development, release and rollback workflow](docs/RELEASE-WORKFLOW.md)
- [CI test strategy](docs/AUTOMATED-TESTS.md)
- [Repository readiness checklist](docs/REPOSITORY-CHECKLIST.md)
- [External component inventory](docs/COMPONENT-INVENTORY.md)
- [Next-stage microSD asset roadmap](docs/SD-ASSET-ROADMAP.md)
- [Hardware and pinout](docs/HARDWARE.md)
- [Security and contribution guidance](docs/SECURITY-AND-CONTRIBUTING.md)

## Source layout

```text
components/
  atilla_storage/          ESPHome component + FatFs read-only transport
  atilla_radar_filter/     RainViewer precipitation palette adapter
docs/                      Current and historical engineering notes
tests/                     ESPHome/SD diagnostic smoke configurations
.github/workflows/         Automated validation experiments
experimental/              Non-production diagnostics and prototypes
```

**Not yet checked into this repository:** the private firmware YAML, the in-YAML weather/CARTO/radar worker implementations, SD-loaded dashboard assets, or a reproducible full firmware build. Do not imply that `tests/fatfs-smoke.yaml` tests the full device.

## Component versioning

The 4.7.9t firmware references `dev/sd-batch-worker-479q` for `atilla_storage` and `dev/radar-intensity` for the radar filter. The storage branch contains the 24 KB worker routing verified on hardware. Prefer **immutable commit references** for future production releases once the integration has been audited; `refresh: 0s` does not make a mutable branch immutable.

## Operating rules

1. Preserve the hardware-validated 4.7.9t private firmware as rollback; don't refactor it and storage internals simultaneously.
2. Never publish access tokens, CARTO keys, Wi-Fi data, Home Assistant secrets or private coordinates.
3. No automatic SD formatting. Read-only behavior by default; any future writes require a separate explicit design, test and recovery plan.
4. Keep LVGL mutations on the ESPHome main loop; network, filesystem and intensive operations run on appropriate workers.
5. Every hardware acceptance claim must be supported by boot logs; distinguish YAML parse, ESPHome validation, compile, OTA and hardware runtime tests.

This branch reorganizes documentation and release discipline only. It **does not change the running device firmware**.
