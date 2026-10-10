# Security and contribution policy

This repository is public to support ESPHome builds that fetch the ATILLA external component.

## Never commit

- Real Wi-Fi SSID/password
- ESPHome API encryption keys, OTA passwords or Home Assistant tokens
- API/map service tokens embedded in tile URLs
- Private dashboard configuration containing those values
- Logs with identifying internal network details or secrets

Use placeholders in examples and Home Assistant ESPHome `secrets.yaml` / `!secret` for private configuration wherever practical. If a secret has already been disclosed, rotate it with the relevant provider.

## Changes

Document hardware revision, ESPHome/ESP-IDF version, expected behavior, safety impact and test outcome. Do not represent proposed changes as tested. Prefer small changes with an easy rollback. The live project may contain user-specific IDs; never silently reconfigure real devices.

## Storage safety

No automatic FAT32 format, no raw writes to allocated sectors, no unbounded busy loops, and no SD writes until driver validation and explicit rollout. Read-only behavior is the safe default.

## Third-party assets

Map/radar services retain their own terms, attribution rules and availability constraints. Don't publish API keys or mirror third-party assets here.
