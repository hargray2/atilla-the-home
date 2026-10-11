# ATILLA repository readiness checklist

Checkpoint: private single-file firmware 4.7.9u; external SD component worker verified with 24 KB stack.

## Currently available
- [x] ESPHome custom component source (`components/atilla_storage`, Python schema, C++ and FatFs adapter).
- [x] Read-only batch probe compile fixture using **local** component source.
- [x] Regression tests for FatFs mount prerequisite, mode exclusion and worker dispatch.
- [x] GitHub Actions validation and compile workflow pinned to ESPHome 2026.9.1.
- [x] Current architecture, hardware acceptance record, release workflow and SD roadmap.
- [x] Private build patterns in `.gitignore`.

## Required before integrating a fully reproducible private build
- [ ] Sanitize full `waveshare.yaml` to generate a **public example without any secrets**, if desired; the canonical complete private YAML remains outside GitHub.
- [x] Import and inventory the two known external components (`atilla_storage`, `atilla_radar_filter`) in this overhaul branch; confirm any additional component references against the complete private YAML before release.
- [x] Record immutable verified *source* commit SHAs in `release/components-4.7.9t.lock.json`. [ ] Update the **private YAML itself** to use pinned references only after separate full-configuration compilation and device testing.
- [ ] Decide how to retain private full firmware backups and release artifacts separately from the public repo.
- [ ] Add validation tests for production-equivalent Waveshare SPI/CH422G wiring (without connecting physical hardware in CI).
- [ ] Add bounded read-only asset discovery/streaming tests before implementing UI SD assets.
- [x] Record 4.7.9u's observed panel compile, boot and early runtime acceptance. [ ] Capture full overhaul-branch Actions pass and run a longer panel soak before promotion.

## CI truth table
A green compile fixture means the *storage adapter* compiles for a generic ESP32-S3 test target. It does not prove the full private dashboard compiles or runs. Card operations in the fixture cannot validate the real Waveshare SD wiring. Never flash the CI fixture to the panel.

## Operational rule
Do not merge this overhaul branch to `main` merely because documents and tests have landed. Review its permissions/security posture, check all Actions outcomes and validate the external-component path first.
