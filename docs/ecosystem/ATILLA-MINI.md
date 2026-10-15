# ATILLA Mini — compact CYD touchscreen

ATILLA Mini is a separate ESPHome firmware project for the **ESP32-2432S028 Cheap Yellow Display (CYD)**. It explores responsive touch navigation, Home Assistant entities, weather, screen retention protection and bounded SD-card diagnostics. It is **not** the 7-inch Waveshare firmware.

## Firmware and evidence

The Mini's versioned source is maintained on the [`dev/cyd-mini-bringup` branch](https://github.com/hargray2/atilla-the-home/tree/dev/cyd-mini-bringup/experimental/atilla-mini). The most recent versioned source in the inspected branch is `atilla-mini-1.5.7-idle-display.yaml`. Earlier YAML revisions represent real engineering milestones, not files to delete without checking rollback dependencies.

The [1.5.7 physical-hardware acceptance report](https://github.com/hargray2/atilla-the-home/blob/dev/cyd-mini-bringup/docs/CYD-MINI-1.5.7-HARDWARE-ACCEPTANCE.md) records background weather, idle dimming, backlight-off and maintained Wi-Fi. It does not establish all first-touch wake, installed-SD-card checks or every refresh behavior.

On October 15, 2026, the registered `atilla-mini` target passed remotely queued `validate` and `build` jobs on ATILLA-LAB with hardware writes disabled. The job summary did not include its exact YAML path; the build therefore does **not** independently prove a particular 1.5.7 source file was compiled.

## Build safely

Use the [versioned source and development README](https://github.com/hargray2/atilla-the-home/blob/dev/cyd-mini-bringup/experimental/atilla-mini/README.md) with local private `secrets.yaml`. ESPHome `config` and `compile` are build-only commands. Do not treat either as permission for OTA/serial upload, device reset, flash erase, or electrical testing.

Keep full factory backups and private credentials outside Git. [ATILLA-LAB](ATILLA-LAB.md) can handle registered builds; any physical writes require separate board-matched authorization and recovery checks.

[Back to ecosystem](README.md).
