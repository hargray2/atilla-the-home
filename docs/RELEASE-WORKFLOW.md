# Release and rollback process

## Branch model

- `main`: conservative/public baseline; change only after hardware sign-off and review.
- `refactor/repository-overhaul-479t`: documentation and repository-structure overhaul; does not modify runtime behavior.
- `dev/sd-batch-worker-479q`: hardware-verified read-only SD worker with 24 KB task stack.
- `dev/radar-intensity`: radar palette external component referenced by the device.
- Other experiment branches are not production checkpoints.

## Release gates

1. Record original full private YAML, component Git commit IDs, ESPHome and ESP-IDF versions.
2. Run syntax and secret scans locally. YAML parse alone is not ESPHome validation.
3. Run ESPHome Validate (without relying on cached validation when testing component changes).
4. Compile full firmware; preserve compiler output.
5. Flash/OTA and verify API handshake, screen, input, Wi-Fi.
6. Verify weather current+forecast, six CARTO tiles, radar metadata+all six tiles, read-only SD mount/open/CRC/enumerate/unmount.
7. Check worker stack high-water, internal heap, PSRAM and component-operation delays. Perform a longer soak before promoting.
8. Promote by PR after hardware evidence; tag immutable commit. Preserve known-good rollback.

## Private firmware workflow

The device installation remains **one complete `waveshare.yaml`**, often delivered as a `.txt` file for copying. This public repo should contain code safe to publish and *sanitized* reproducibility guidance—not the user-specific firmware source containing keys.

**No untested SD writes.** Dedicated write experiments must be opt-in and isolated from normal dashboard execution.

## Failure response

- Network/weather regressions: keep the last successful weather state when safe; roll back to complete 4.7.9t if needed.
- SD mount or CRC mismatch: abort read; never format, erase or auto-repair.
- Boot loop, memory regression or UI stall: roll back firmware and pin external components to prior known commits.
