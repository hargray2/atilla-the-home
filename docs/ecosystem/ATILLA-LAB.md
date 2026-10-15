# ATILLA-LAB — autonomous ESP development station

ATILLA-LAB is the local engineering and test system behind the ATILLA devices. It runs on a **Dell Latitude E6420 with Debian 13/Xfce**, hostname `atilla-lab`, and is being developed as a general-purpose ESPHome/PlatformIO firmware workstation rather than a single-display controller.

Its installed source, private board identities, credentials and firmware artifacts reside in a separate checkout on the Dell; **they are not published in this firmware-components repository**.

## Verified capabilities, October 15, 2026

- Registered ATILLA-Mini firmware target, toolchains and persistent queued jobs.
- Authenticated, time-limited ChatGPT connector for development jobs and read-only job history.
- Native `validate` and `build` tasks for the registered ATILLA-Mini target completed successfully without a local shell command from the user. The build returned a source digest, but the remote job result did not include the exact YAML filename or bounded compiler log.
- Separately qualified server-source patching and authorized deployment with service restart/health confirmation.
- Working V4L2 camera registered at **1920 × 1080 MJPEG**. A complete visual firmware acceptance service has not yet been demonstrated.
- Firmware flashing, erasing and hardware writes **disabled** through the connector.

## Direction and boundaries

The lab is intended to automate inspection, builds, diagnostic collection, physical verification, and eventually board-specific authorized recovery. Other registered targets, appliance retrofits, Tuya MCU bridges and Home Assistant dashboard workflows must be tested separately before describing them as production-ready.

Read-only Home Assistant discovery and a local Lovelace JSON export have been demonstrated previously. This does not permit live HA configuration changes or appliance controls. Planned one-click HA setup and automatic hardware flashing must not be described as shipped.

Local AI reservations use an internal spending ceiling; it is not a provider-side billing cutoff. Local validation/build jobs are distinct from paid AI development runs.

**Hardware boundaries:** never publish tokens or original flash backups; never assume a build success grants flashing. Require matching board identity, explicit authorization, validated recovery and post-write testing for any hardware change.

[Back to ecosystem](README.md) · [Mini](ATILLA-MINI.md) · [integration](INTEGRATION.md).
