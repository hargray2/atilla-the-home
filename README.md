# ATILLA — ESP smart-home & firmware-development ecosystem

**ATILLA connects touchscreen smart-home interfaces, reusable ESPHome components, and a dedicated development lab.** It is a growing collection of related projects—not just one Waveshare display.

ATILLA is developed on real ESP hardware, with separate source histories, build checks and hardware acceptance for each device. This public repository holds reusable components, development firmware, tests and engineering documentation; some complete device configurations and the local development server are intentionally private or maintained separately.

## Explore the projects

| Project | What it does | Start here |
|---|---|---|
| **ATILLA Waveshare 7-inch** | ESP32-S3 / LVGL home dashboard with weather, forecasts, CARTO map imagery, RainViewer radar, touch controls and SD-card asset experiments. | [Waveshare overview](docs/ecosystem/WAVESHARE-7-INCH.md) |
| **ATILLA Mini (CYD)** | Independent compact ESP32 touchscreen with weather, Home Assistant integration, touch UI, idle-display protection and peripheral experiments. | [Mini overview](docs/ecosystem/ATILLA-MINI.md) · [versioned Mini firmware](https://github.com/hargray2/atilla-the-home/tree/dev/cyd-mini-bringup/experimental/atilla-mini) |
| **ATILLA-LAB** | Debian-based autonomous ESP development station with registered targets, ESPHome validation/build, persistent jobs, qualified server updates and camera-assisted test infrastructure. | [Lab overview](docs/ecosystem/ATILLA-LAB.md) |

**[Full ecosystem guide](docs/ecosystem/README.md)** · **[Project boundaries](docs/ecosystem/INTEGRATION.md)**

## What is actually in this repository?

```text
components/
  atilla_storage/          ESPHome SD/FatFs transport, bounded readers and opt-in experiments
  atilla_radar_filter/     RainViewer radar precipitation palette/filter component
tests/                     Compile fixtures and offline regression checks
tools/                     Manifest validation and supporting scripts
docs/                      Architecture, dated hardware evidence, release notes and experiments
experimental/              Prototype code and design notes
release/                   Pinned component-revision references
.github/workflows/         Automated component checks
```

The public `main` branch contains the Waveshare component code and the latest documented Waveshare hardware checkpoints. ATILLA Mini's versioned ESPHome YAML is maintained on [`dev/cyd-mini-bringup`](https://github.com/hargray2/atilla-the-home/tree/dev/cyd-mini-bringup). ATILLA-LAB's active server checkout lives on a separate local machine and is **not** published as part of this repository.

**Not included:** the complete private Waveshare `waveshare.yaml`, home network credentials, Home Assistant tokens, original flash backups, or a reproducible public build of the whole 7-inch dashboard. Component smoke-test YAML is **not** a substitute for the complete private firmware.

## Current verified milestones

| Project | Documented checkpoint | Evidence boundary |
|---|---|---|
| Waveshare | **4.7.9aa**: opt-in 70-byte immutable PNG cache write, CRC verification and second-boot persistence; weather and six map/radar tiles also observed | [4.7.9aa hardware record](docs/VALIDATED-BASELINE-4.7.9aa-WRITE-FIRST-PASS.md). A single verified pilot does **not** establish general SD writes, power-loss safety or SD graphics rendering |
| Waveshare | **4.7.9z**: read-only manifest, bounded file CRC and reader checks | [4.7.9z hardware record](docs/VALIDATED-BASELINE-4.7.9z.md) |
| Mini | **1.5.7**: versioned source and documented idle-dimming/backlight-off behavior | [Mini acceptance evidence](https://github.com/hargray2/atilla-the-home/blob/dev/cyd-mini-bringup/docs/CYD-MINI-1.5.7-HARDWARE-ACCEPTANCE.md). Some touch-wake/SD cases remain unverified |
| ATILLA-LAB | Remotely queued `validate` and `build` jobs succeeded for registered Mini target on 2026-10-15 | The remote result did not expose the exact YAML filename. Builds do not imply a flash or complete physical acceptance |

See [release notes](docs/RELEASE-NOTES.md) and the [latest repository audit](https://github.com/hargray2/atilla-the-home/blob/audit/atilla-ecosystem-20261015/docs/REPOSITORY-AUDIT-2026-10-15.md) for engineering history and unresolved issues.

## Build and development

This repository contains [documented external components](docs/COMPONENT-INVENTORY.md), [CI smoke fixtures](docs/AUTOMATED-TESTS.md), and [build/troubleshooting guidance](docs/BUILD-AND-TROUBLESHOOTING.md). Use the appropriate board configuration and `secrets.yaml` outside Git, and distinguish **source review → ESPHome validation → compile → authorized deployment → observed hardware behavior**.

This is an active engineering project. Experimental branches and historical tests are preserved intentionally for provenance; a draft pull request is not a production release.

## Safety and contributions

- Never commit Wi-Fi secrets, encryption keys, tokens, real private coordinates, device backups or the complete private panel firmware.
- Never flash a CI smoke fixture. Hardware writes require a matching board identity, a known-good recovery image and explicit authorization.
- Do not enable SD-card write experiments, formatting or appliance-control changes by default.
- Read [security and contribution policy](docs/SECURITY-AND-CONTRIBUTING.md) and [release workflow](docs/RELEASE-WORKFLOW.md) before testing on physical equipment.

**Reuse / licensing:** No license grant is currently published here. Read access to a public repository does not by itself grant unrestricted reuse or commercial rights. Licensing terms require a separate maintainer decision.
