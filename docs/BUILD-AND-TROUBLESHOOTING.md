# ESPHome build and troubleshooting — current summary

ATILLA is built with Home Assistant ESPHome Builder using a single private `waveshare.yaml`; `external_components` loads public custom components from GitHub at build time.

## Build and source-control discipline

- Keep private full-device configuration, credentials, map service tokens and device-specific values off public GitHub.
- Prefer a pinned known-good commit for reproducible builds; `refresh: 0s` requests fresh external-source retrieval.
- A YAML structural pass is **not** a C++ compilation pass. A compile is **not** a successful OTA/hardware test.
- Keep the last hardware-tested rollback available; suffix experimental candidates with letters and advance numeric versions only after functional validation.
- Capture the **first** meaningful error, current ESPHome version, component ref and whether the failure occurred during validation, compilation, OTA, or runtime.

## Known technical findings

| Issue | Finding / status |
|---|---|
| Python `spi_device` codegen type | Use `spi_device.spi_device` for ESPHome versions used in ATILLA; prior AttributeError corrected. |
| C++ namespace ambiguity | Transport must be declared consistently under `esphome::atilla_storage`. |
| Read-only SD | Hardware-verified; current `main` component is not writable. |
| RainViewer dispatch | `online_image.set_url` synchronously blocked ~3.1–3.5 s per tile in instrumented runs. |
| Existing PNG decode | Can stall the main loop for hundreds of milliseconds. |
| Dedicated HTTPS task | 4.7.5c returned HTTP 200 without a matching multi-second main-loop warning. |
| Full PNG worker | 4.7.6 downloaded PNG into bounded PSRAM; 4.7.7 queue ownership handoff passed. |
| PNG decoder diagnostic | 4.7.8b–c compiled/ran but `pngle_new()` failed. 4.7.8d PSRAM decoder candidate is untested. |

### Example worker evidence

```text
PNG FULL BODY PASS http=200 bytes=26176 expected=26176 complete=1 signature=1 iend=1
worker queued PNG bytes=26176 for main-loop validation
MAIN LOOP PNG HANDOFF PASS bytes=26176 width=256 height=256; releasing PSRAM
```

### 4.7.8c allocator finding

```text
before pngle: internal_free=100812 internal_max=31744
              psram_free=4742544 psram_max=4718592
pngle_new=FAIL
RGB565 PSRAM frame=SKIPPED
DECODE SKIPPED
```

Do **not** treat that as a successful decoded frame: no RGB565 frame was produced. The tested alternative is to allocate PNGLE decoder state in PSRAM, pending 4.7.8d compile and board logs.

## Operational warnings

The worker request can still create short scheduling warnings and uses temporary internal heap. This experimental probe has **not** replaced the existing six `online_image` radar fetches, so long scheduler warnings remain expected until the actual rendering pipeline is migrated. Never eliminate warnings by raising thresholds rather than fixing the causes.

Full log URLs can expose API keys or private coordinates; redact before sharing. Keep RGB pixel clock and radar pacing consistent with the tested hardware until changes are measured.
