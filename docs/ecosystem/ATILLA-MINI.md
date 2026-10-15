# ATILLA Mini — independent CYD touchscreen

**Role:** a compact ESP32 touchscreen endpoint with its own ESPHome firmware, navigation, weather interface and SD experiments. Mini is not a scaled copy of the Waveshare 7-inch dashboard.

**Source:** [versioned firmware and build guide](../../experimental/atilla-mini/README.md) on `dev/cyd-mini-bringup`. The most recent versioned source carried in that branch is `experimental/atilla-mini/atilla-mini-1.5.7-idle-display.yaml`. Earlier source snapshots remain available to preserve engineering history.

**Hardware evidence:** [Mini 1.5.7 acceptance](../CYD-MINI-1.5.7-HARDWARE-ACCEPTANCE.md) documents background weather, screen-protection dimming, backlight-off idle and continued connectivity/uptime. Touch wake and insertion-dependent SD verification are not established by that report.

**Build evidence (2026-10-15):** ATILLA-LAB successfully queued an ESPHome `validate` job and an ESPHome `build` job for its registered `atilla-mini` target. The remote summaries returned success and a source digest, **but no exact YAML filename**; therefore the result establishes a registered target build, not independent proof of a particular 1.5.7 commit. The exact source and private `!secret` values should be verified before release.

**Hardware boundary:** flashing, erasing, serial writes, backups and bootloader recovery are different operations from building. All remote hardware writes remain disabled. Keep a verified original flash backup private, match the physical board ID and obtain explicit authorization before deploying any firmware. Firmware belonging to Mini must never implicitly change Waveshare configuration.

**Related:** [ecosystem overview](README.md) · [ATILLA-LAB](ATILLA-LAB.md) · [hardware integration rules](INTEGRATION.md).
