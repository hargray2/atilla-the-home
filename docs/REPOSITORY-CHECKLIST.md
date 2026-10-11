# ATILLA repository readiness checklist

Checkpoint: private single-file firmware 4.7.9t; external SD component worker verified with 24 KB stack.

## Currently available
- [x] ESPHome custom component source (`components/atilla_storage`, Python schema, C++ and FatFs adapter).
- [x] Read-only batch probe compile fixture using **local** component source.
- [x] Regression tests for FatFs mount prerequisite, mode exclusion and worker dispatch.
- [x] GitHub Actions validation and compile workflow pinned to ESPHome 2026.9.1.
- [x] Current architecture, hardware acceptance record, release workflow and SD roadmap.
- [x] Private build patterns in `.gitignore`.

## Required before integrating a fully reproducible private build
- [ ] Sanitize full `waveshare.yaml` to generate a **public example without any secrets**, if desired; the canonical complete private YAML remains outside GitHub.
- [ ] Inventory custom ESPHome components referenced by the private YAML, including radar filter and other optional components. The repo currently includes storage only on this branch.
- [ ] Use pinned Git commit SHAs or release tags for component versions; floating development branches may change under a known-good firmware.
- [ ] Decide how to retain private full firmware backups and release artifacts separately from the public repo.
- [ ] Add validation tests for production-equivalent Waveshare SPI/CH422G wiring (without connecting physical hardware in CI).
- [ ] Add bounded read-only asset discovery/streaming tests before implementing UI SD assets.
- [ ] Record successful Actions compile, panel OTA and longer hardware soak for each promoted release.

## CI truth table
A green compile fixture means the *storage adapter* compiles for a generic ESP32-S3 test target. It does not prove the full private dashboard compiles or runs. Card operations in the fixture cannot validate the real Waveshare SD wiring. Never flash the CI fixture to the panel.

## Operational rule
Do not merge this overhaul branch to `main` merely because documents and tests have landed. Review its permissions/security posture, check all Actions outcomes and validate the external-component path first.
