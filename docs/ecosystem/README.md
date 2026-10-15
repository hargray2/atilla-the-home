# ATILLA — ESP development ecosystem

ATILLA is a family of independent ESP devices **and** the local engineering infrastructure used to build and test them. The three current projects have separate source, hardware identities, acceptance evidence and permissions.

| Project | Role | Evidence and source |
|---|---|---|
| [ATILLA-LAB](ATILLA-LAB.md) | Debian-based multi-target development, native firmware validation/build, persistent jobs and camera-assisted diagnostics | Remote validate/build of a registered Mini target was verified 2026-10-15; firmware writes remain disabled |
| [Waveshare 7-inch](WAVESHARE-7-INCH.md) | Full-size ESP32-S3/LVGL home dashboard for weather, maps, radar and storage experiments | Private complete firmware; public external components and historical hardware records; main/docs branches have diverged |
| [ATILLA Mini](ATILLA-MINI.md) | Independent ESP32 CYD touchscreen display, weather and SD/peripheral experiments | [Versioned Mini source](../../experimental/atilla-mini/README.md) through 1.5.7; hardware acceptance has limitations |

## Where the material lives

- `components/`: public Waveshare external ESPHome components, not the complete private `waveshare.yaml`.
- `experimental/atilla-mini/`: independent Mini YAML snapshots and a current source/version guide.
- `tests/`, `tools/`, `.github/workflows/`: component smoke tests, offline checks and CI.
- `docs/`: release notes, dated acceptance records, experiments and current `docs/ecosystem/` descriptions.
- ATILLA-LAB's working server installation is kept **outside** this public firmware repository, on the Dell.

## Release and safety boundaries

Consult the [integration and ownership rules](INTEGRATION.md), [repository audit of 2026-10-15](../REPOSITORY-AUDIT-2026-10-15.md), and [every-file inventory](../FILE-INVENTORY-2026-10-15.md). The original ecosystem proposal is [PR #14](https://github.com/hargray2/atilla-the-home/pull/14), still under review. The default `main` branch has different Waveshare storage milestones than the Mini development branch; do not assume these are merged or hardware-qualified together.

Device source editing, configuration validation, compilation, deployment, physical acceptance and rollback are separate stages. A compile neither authorizes flashing nor proves the screen displays the expected output. Never publish secrets, private firmware, original flash backups, local network identifiers or private location information. Home Assistant read-only discovery does not imply authorization to control appliances.
