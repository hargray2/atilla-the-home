# External component inventory — 4.7.9t

## In this overhaul branch

| Component | Source | Gate | Device |
|---|---|---|---|
| `atilla_storage` | Imported from hardware-tested `dev/sd-batch-worker-479q` | Local FatFs smoke fixture + mode tests | Verified in 4.7.9t logs |
| `atilla_radar_filter` | Imported unchanged from `dev/radar-intensity` | Palette self-test | Radar display/filter pipeline verified in 4.7.9t logs |

The imported radar source still references ESPHome `OnlineImage` APIs. Its existing smoke fixture on the source branch uses ESPHome 2026.10.0b2, whereas the deployed panel runs 2026.9.1. Therefore a separate radar **palette** workflow is enabled now, but full radar component compilation under a production-matching ESPHome version must be proven before enabling it as a merge gate. No claim of independent compilation is made for the newly imported component.

## Production integration prerequisites

- The authoritative full private `waveshare.yaml` is intentionally not committed. It includes inline C++ worker implementations and sensitive endpoint details.
- Need a sanitized component manifest with exact commit SHA versions used for each firmware release.
- Decide whether weather/map/radar workers should remain inline in the single-file private YAML or be extracted into dedicated components. Extraction needs a full compile+hardware regression gate.
- Do not re-point the live panel to the overhaul branch until its component compilation and OTA tests succeed.
