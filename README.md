# ATILLA — ESP Development Ecosystem

**ATILLA is an open, evolving ecosystem for ESP-based smart devices and the automated tools used to build, test, flash, and maintain them.** It is **not just a Waveshare display project**. The three current projects are ATILLA-LAB, the Waveshare 7-inch display, and ATILLA Mini (Cheap Yellow Display / CYD).

The ecosystem combines local ESPHome development, Home Assistant integration, hardware testing, weather/radar interfaces and a growing toolkit for Tuya MCU bridges and retrofitted smart appliances. Each project has its own source, hardware constraints, testing evidence and release process.

## The three current ATILLA projects

### 1. ATILLA-LAB — Automated ESP development server

**The engineering environment behind the ecosystem.** A Debian-based development server on a Dell Latitude E6420 that is being built to work with ATILLA **and other ESPHome projects**.

- Locally validates, compiles and qualifies ESPHome firmware; keeps normal builds off paid AI APIs.
- Manages registered boards, hardware identities, backups, source revisions, build artifacts and persistent jobs.
- Provides controlled, allowlisted, job-specific flashing infrastructure, with post-flash verification and recovery automation under development. **Fully autonomous flashing and erasing are not yet hardware-approved.**
- Provides an authenticated localhost management dashboard and a read-only Home Assistant discovery/dashboard-design workflow. Live discovery of 800+ entities and local Lovelace JSON export have been demonstrated.
- Is being extended for Tuya MCU UART devices, appliance retrofits, serial/camera-assisted tests, and future toolchains.

**[ATILLA-LAB documentation](docs/ecosystem/ATILLA-LAB.md)** · [Integration boundaries](docs/ecosystem/INTEGRATION.md)

### 2. ATILLA Waveshare 7-inch — Touchscreen home dashboard

An ESPHome/LVGL smart-home and weather/radar dashboard on the **Waveshare ESP32-S3-Touch-LCD-7 Rev 1.2**, with an 800 × 480 RGB display, GT911 touch, PSRAM and microSD.

- Weather and forecasts, CARTO basemap imagery and RainViewer radar.
- Custom read-only SD/FatFs diagnostics and asset experiments.
- Latest **documented** Waveshare hardware checkpoint on the inspected development branch: **4.7.9y**, with weather/radar/display and combined read-only file checks.
- The complete authoritative `waveshare.yaml` is **private**, not included here; public external components and test fixtures are not a reproducible full panel build.

**[Waveshare project documentation](docs/ecosystem/WAVESHARE-7-INCH.md)** · [Hardware acceptance 4.7.9y](docs/VALIDATED-BASELINE-4.7.9y.md) · [Wiring and hardware](docs/HARDWARE.md)

### 3. ATILLA Mini — Cheap Yellow Display (CYD)

A separate compact ESP32 touchscreen running its own ESPHome firmware, designed as a smaller ATILLA interface and experimental instrument/display platform.

- Source: [ATILLA Mini ESPHome project](experimental/atilla-mini/).
- Hardware acceptance for 1.5.7 documents background weather retrieval, anti-retention dimming and backlight-off idle behavior.
- Developed and built through ATILLA-LAB without modifying Waveshare firmware.
- Some Mini touch-wake and SD-card test cases remain unverified; build success is not the same as complete hardware acceptance.

**[Mini project documentation](docs/ecosystem/ATILLA-MINI.md)** · [Mini 1.5.7 acceptance](docs/CYD-MINI-1.5.7-HARDWARE-ACCEPTANCE.md)

## Repository layout

This public repository currently contains more **device firmware components and history** than ATILLA-LAB server source. The lab is maintained in a separate Dell checkout; it must not be treated as already published in this repo.

```text
components/                  Public Waveshare external components
experimental/atilla-mini/    ATILLA Mini firmware and experiments
tests/                       Component and firmware smoke fixtures
release/                     Component lockfile and release references
docs/ecosystem/              Three-project ecosystem entry points
docs/                        Historical technical reports and acceptance evidence
.github/workflows/           Automated validation experiments
```

**[Explore the ecosystem docs](docs/ecosystem/README.md)** · [Build and release workflow](docs/RELEASE-WORKFLOW.md) · [Repository checklist](docs/REPOSITORY-CHECKLIST.md) · [Security guidance](docs/SECURITY-AND-CONTRIBUTING.md)

## Engineering principles

- Keep each physical device's firmware, documentation, acceptance tests and rollback independent.
- Never publish private full-firmware YAML, Wi-Fi credentials, tokens, Home Assistant secrets, private device backups or location information.
- Validate and build locally; require verified device identity, project-specific authorization and recovery readiness before hardware writes.
- Treat UI screenshots, successful compilation, successful OTA and verified physical behavior as **different levels of evidence**.
- Keep unsafe appliance-control experiments and SD writes behind explicit hardware-specific safeguards.

## Support ATILLA development

ATILLA is a hobbyist ecosystem developed and tested with real ESP hardware. If it has been useful to you, voluntary donations help cover development boards, displays, test equipment and ongoing project work. Donations are appreciated but are not required to use the publicly available materials.

- **Cash App:** [Donate via Cash App](https://cash.app/$danielkharvey) (`$danielkharvey`)
- **PayPal:** [Donate with PayPal](https://www.paypal.com/qrcodes/p2pqrc/TFBQ3QZKZV8FG) (QR payment link verified against the provided original image), or use `hargray2@yahoo.com` in PayPal's **Send** feature.

[![Scan to support ATILLA with PayPal](assets/paypal-donation-qr.svg)](https://www.paypal.com/qrcodes/p2pqrc/TFBQ3QZKZV8FG)

*PayPal donation QR (recreated as a scannable vector code for the same verified destination).*

Please verify the recipient within your payment app before sending money. These are personal voluntary-support methods, not purchases or a promise of support/services.

*This branch updates the ecosystem documentation and project presentation; it does not change deployed firmware or the running development server.*
