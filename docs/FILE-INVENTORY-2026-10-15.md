# File-by-file inventory — 2026-10-15

**Review branch:** `audit/atilla-ecosystem-20261015` (based on existing ecosystem PR #14).
**Scope:** 105 tracked blobs at snapshot plus this inventory file (106 total upon creation). Files were read on the base ecosystem documentation branch; new review changes have been inspected individually. Branch history and every alternate HEAD's full bytes are not covered by this inventory.

Disposition is conservative: **preserve** unless a dependency and history/rollback check proves removal is safe. It is not a test-pass claim. See [audit findings](REPOSITORY-AUDIT-2026-10-15.md).

| Path | Audit role / disposition |
|---|---|
| `.github/FUNDING.yml` | Public support asset — review recipient before merge |
| `.github/workflows/asset-manifest.yml` | CI workflow — keep; verify actual Actions result |
| `.github/workflows/fatfs-smoke.yml` | CI workflow — keep; verify actual Actions result |
| `.github/workflows/integration-components.yml` | CI workflow — keep; verify actual Actions result |
| `.github/workflows/pinned-remote.yml` | CI workflow — keep; verify actual Actions result |
| `.github/workflows/radar-component.yml` | CI workflow — keep; verify actual Actions result |
| `.github/workflows/radar-palette.yml` | CI workflow — keep; verify actual Actions result |
| `.github/workflows/repository-docs.yml` | CI workflow — keep; verify actual Actions result |
| `.gitignore` | Private-material exclusion rules — keep |
| `README.md` | Current ecosystem entry page |
| `assets/manifest.example.json` | Example/reference asset — preserve |
| `assets/paypal-donation-qr.svg` | Public support asset — review recipient before merge |
| `components/atilla_radar_filter/__init__.py` | External ESPHome component source — preserve; hardware/branch-gated |
| `components/atilla_radar_filter/atilla_radar_filter.h` | External ESPHome component source — preserve; hardware/branch-gated |
| `components/atilla_storage/README.md` | External ESPHome component source — preserve; hardware/branch-gated |
| `components/atilla_storage/__init__.py` | External ESPHome component source — preserve; hardware/branch-gated |
| `components/atilla_storage/atilla_storage.h` | External ESPHome component source — preserve; hardware/branch-gated |
| `components/atilla_storage/fatfs_readonly.h` | External ESPHome component source — preserve; hardware/branch-gated |
| `docs/ARCHITECTURE.md` | Architecture/release/security documentation — review dated status |
| `docs/AUTOMATED-TESTS.md` | Architecture/release/security documentation — review dated status |
| `docs/BOUNDED-CRC-READ-PROBE.md` | Technical milestone/experiment document — preserve |
| `docs/BUILD-AND-TROUBLESHOOTING.md` | Architecture/release/security documentation — review dated status |
| `docs/COMPONENT-INVENTORY.md` | Architecture/release/security documentation — review dated status |
| `docs/CURRENT-ARCHITECTURE.md` | Architecture/release/security documentation — review dated status |
| `docs/CYD-ATILLA-MINI-BRINGUP.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-0.5.0-INSTRUMENT-DECK.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-0.7.0-NOTES.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-0.8.0-BIRTHDAY-BLITZ.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-0.9.0-BIRTHDAY-CHALLENGE.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.0.0-ROOM-CONTROL.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.1.0-SANDBOX.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.2.0-PIXEL-FORGE.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.3.0-PERFORMANCE-LAB.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.4.0-TWO-BANK-NAV.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.4.1-ESP-IDF-OPTIMIZATIONS.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.4.4-CENTERED-PILLS.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.4.5-SCOPE.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.4.6-LIVE-FORECAST.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.5.0-ASSET-VAULT.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.5.1-USEFUL-HOME.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.5.2-HARDWARE-ACCEPTANCE.md` | Hardware acceptance evidence — preserve original |
| `docs/CYD-MINI-1.5.2-SD-COMPILE-FIX.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.5.3-HARDWARE-ACCEPTANCE.md` | Hardware acceptance evidence — preserve original |
| `docs/CYD-MINI-1.5.3-ROOT-SECTOR-AND-UI-TUNING.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.5.4-KNOWN-FILE-CRC.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.5.4-NO-SD-HARDWARE-ACCEPTANCE.md` | Hardware acceptance evidence — preserve original |
| `docs/CYD-MINI-1.5.5-RUNTIME-AND-1.5.6-TOUCH-TUNING.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.5.5-TOUCH-RESPONSIVENESS.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-1.5.7-HARDWARE-ACCEPTANCE.md` | Hardware acceptance evidence — preserve original |
| `docs/CYD-MINI-1.5.7-IDLE-DISPLAY.md` | Technical milestone/experiment document — preserve |
| `docs/CYD-MINI-SD-ROADMAP.md` | Technical milestone/experiment document — preserve |
| `docs/HARDWARE.md` | Architecture/release/security documentation — review dated status |
| `docs/READONLY-ASSET-READER-API.md` | Technical milestone/experiment document — preserve |
| `docs/RELEASE-NOTES.md` | Architecture/release/security documentation — review dated status |
| `docs/RELEASE-WORKFLOW.md` | Architecture/release/security documentation — review dated status |
| `docs/REPOSITORY-AUDIT-2026-10-10.md` | Historical/current audit record — preserve |
| `docs/REPOSITORY-AUDIT-2026-10-14.md` | Historical/current audit record — preserve |
| `docs/REPOSITORY-AUDIT-2026-10-15.md` | Historical/current audit record — preserve |
| `docs/REPOSITORY-CHECKLIST.md` | Snapshot checklist — historical status |
| `docs/ROADMAP.md` | Architecture/release/security documentation — review dated status |
| `docs/SD-ASSET-DISCOVERY-PROBE.md` | Technical milestone/experiment document — preserve |
| `docs/SD-ASSET-MANIFEST.md` | Technical milestone/experiment document — preserve |
| `docs/SD-ASSET-ROADMAP.md` | Technical milestone/experiment document — preserve |
| `docs/SD-VALIDATION.md` | Technical milestone/experiment document — preserve |
| `docs/SECURITY-AND-CONTRIBUTING.md` | Architecture/release/security documentation — review dated status |
| `docs/STORAGE-4.6.md` | Technical milestone/experiment document — preserve |
| `docs/VALIDATED-BASELINE-4.7.9t.md` | Hardware acceptance evidence — preserve original |
| `docs/VALIDATED-BASELINE-4.7.9u.md` | Hardware acceptance evidence — preserve original |
| `docs/VALIDATED-BASELINE-4.7.9v.md` | Hardware acceptance evidence — preserve original |
| `docs/VALIDATED-BASELINE-4.7.9w.md` | Hardware acceptance evidence — preserve original |
| `docs/VALIDATED-BASELINE-4.7.9y.md` | Hardware acceptance evidence — preserve original |
| `docs/ecosystem/ATILLA-LAB.md` | Current project overview / integration boundary |
| `docs/ecosystem/ATILLA-MINI.md` | Current project overview / integration boundary |
| `docs/ecosystem/INTEGRATION.md` | Current project overview / integration boundary |
| `docs/ecosystem/README.md` | Current project overview / integration boundary |
| `docs/ecosystem/WAVESHARE-7-INCH.md` | Current project overview / integration boundary |
| `experimental/RADAR-FILTER.md` | Experimental source/notes — keep isolated |
| `experimental/README.md` | Experimental source/notes — keep isolated |
| `experimental/atilla-mini/NEO-0.3.0-NOTES.md` | Experimental source/notes — keep isolated |
| `experimental/atilla-mini/README.md` | Current Mini project entry page |
| `experimental/atilla-mini/atilla-mini-0.1.0-st7789v.yaml` | Versioned Mini firmware snapshot — preserve for reproducibility |
| `experimental/atilla-mini/atilla-mini-0.1.1-sntp.yaml` | Versioned Mini firmware snapshot — preserve for reproducibility |
| `experimental/atilla-mini/atilla-mini-0.2.0-neo.yaml` | Versioned Mini firmware snapshot — preserve for reproducibility |
| `experimental/atilla-mini/atilla-mini-0.2.1-mipi-spi.yaml` | Versioned Mini firmware snapshot — preserve for reproducibility |
| `experimental/atilla-mini/atilla-mini-0.6.0-instrument-deck.yaml` | Versioned Mini firmware snapshot — preserve for reproducibility |
| `experimental/atilla-mini/atilla-mini-1.5.2-hardware-pass.yaml` | Versioned Mini firmware snapshot — preserve for reproducibility |
| `experimental/atilla-mini/atilla-mini-1.5.3-asset-vault-preview.yaml` | Versioned Mini firmware snapshot — preserve for reproducibility |
| `experimental/atilla-mini/atilla-mini-1.5.4-known-file-crc.yaml` | Versioned Mini firmware snapshot — preserve for reproducibility |
| `experimental/atilla-mini/atilla-mini-1.5.5-touch-responsive.yaml` | Versioned Mini firmware snapshot — preserve for reproducibility |
| `experimental/atilla-mini/atilla-mini-1.5.6-touch-latency.yaml` | Versioned Mini firmware snapshot — preserve for reproducibility |
| `experimental/atilla-mini/atilla-mini-1.5.7-idle-display.yaml` | Latest versioned Mini firmware — verify release/build identity |
| `experimental/atilla_fatfs_readonly.h` | Experimental source/notes — keep isolated |
| `release/components-4.7.9t.lock.json` | Immutable component reference/checkpoint — preserve |
| `tests/asset-discovery-smoke.yaml` | Diagnostic/test fixture — preserve, no hardware flash |
| `tests/bounded-crc-smoke.yaml` | Diagnostic/test fixture — preserve, no hardware flash |
| `tests/combined-readonly-smoke.yaml` | Diagnostic/test fixture — preserve, no hardware flash |
| `tests/fatfs-smoke.yaml` | Diagnostic/test fixture — preserve, no hardware flash |
| `tests/integration-remote-pin.yaml` | Diagnostic/test fixture — preserve, no hardware flash |
| `tests/integration-smoke.yaml` | Diagnostic/test fixture — preserve, no hardware flash |
| `tests/radar-filter-smoke.yaml` | Diagnostic/test fixture — preserve, no hardware flash |
| `tests/radar_palette_selftest.py` | Diagnostic/test fixture — preserve, no hardware flash |
| `tests/test_repository_docs.py` | Diagnostic/test fixture — preserve, no hardware flash |
| `tests/test_storage_flags.py` | Diagnostic/test fixture — preserve, no hardware flash |
| `tools/check_repository_docs.py` | Repository validation tool — retain and test |
| `tools/validate_asset_manifest.py` | Repository validation tool — retain and test |
| `docs/FILE-INVENTORY-2026-10-15.md` | This dated tracked-path inventory — keep and regenerate for later audits |

## Scope warning

A file's presence in this table does not mean it is part of a reproducible full firmware release. Firmware YAML snapshots, external components, CI smoke fixtures, and on-hardware acceptance evidence have separate roles. The complete private Waveshare firmware and the live ATILLA-LAB server source are not public files in this checkout.
