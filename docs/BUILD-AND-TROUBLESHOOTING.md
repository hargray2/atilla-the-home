# ESPHome build and troubleshooting

## Installation workflow

ATILLA is managed via iPhone and Home Assistant ESPHome Builder. The production configuration is one large `waveshare.yaml`; GitHub stores additional component source retrieved at **build time**. Do not ask the user to manually copy C++ headers to the ESPHome filesystem.

A representative external-component section is:

```yaml
external_components:
  - source: github://hargray2/atilla-the-home@main
    refresh: 0s
    components: [atilla_storage]
```

`refresh: 0s` requests a fresh Git repository check for every validation/build. Once releases stabilize, pin to a tested Git tag/commit rather than mutable `main`.

## Failure history

### Validation: AttributeError, SPI device class

```text
AttributeError: module 'esphome.components.spi_device'
has no attribute 'SPIDeviceComponent'
```

Cause: Python config schema referenced `spi_device.SPIDeviceComponent`, while ESPHome 2026.9.1 defines the codegen type as `spi_device.spi_device`. Fixed for both `card_device` and `idle_device` on GitHub.

### Clean Build Files failed with the same AttributeError

ESPHome must first load the configuration (including cached external Python modules), so a broken cached module may prevent the Clean command from proceeding. ESPHome 2026.9.1 external components default to a one-day refresh. Set `refresh: 0s` and validate again to retrieve the fix. An unchanged error may mean a stale repository cache or a different ref; confirm the fetched file contents.

### C++ compile: ambiguous `atilla_storage` namespace

```text
error: reference to 'atilla_storage' is ambiguous
```

The original header declared a global `namespace atilla_storage` as well as `namespace esphome::atilla_storage`. Generated `main.cpp` encountered both. The header was updated to put the transport in `esphome::atilla_storage` as well. **The user has not yet reported a successful new build after this fix.**

### TLS connection failures during radar preload

Earlier rapid tile requests caused SSL/setup resource errors or HTTP connection failures. Historical stable transfer gaps were ~1.5 seconds (map) and ~4 seconds (radar); do not tighten them casually.

### OTA rollback

A more aggressive FAT32 directory scan in the 4.0.3 experiment was followed by rollback. Conservatively limited sector reads later succeeded. An OTA rollback cannot alone establish the precise root cause.

## Reporting future errors

Record ESPHome version, current Git commit or tag, first meaningful compiler error (not only Ninja's final failure), and whether Validate, Compile, or OTA Install failed. **Redact secrets, API keys, Wi-Fi credentials, access tokens, private IP information if sensitive, and query parameters from map URLs.**

## 2026-10-10 — radar startup stall and duplicate SD work (4.6.4 private candidate)

Real ATILLA logs show synchronous `fetch_rainviewer_radar` callbacks blocking the ESPHome main loop for ~0.8–6.4 seconds; a weather request also blocked ~5.4 seconds. `online_image.image` PNG decoding takes ~0.2–0.6 seconds. These are separate from the brief horizontal RGB glitches, which the user reports disappeared after setting `mipi_rgb.pclk_frequency: 14MHz`.

The existing private full YAML runs **two independent startup FAT32 readers**: a legacy single-YAML `esphome.on_boot` action at priority -200 (322 ms blocking in the supplied logs), plus the new GitHub `atilla_storage` FATFS mount-only component (419 ms). Both verify the same file CRC32. An optional private **4.6.4 minimal-risk candidate** removes the entire priority -200 legacy one-file FAT32 scan while preserving the GitHub driver, FATFS read-only mount/unmount, the display's 14 MHz clock, six map and six radar tiles, all scheduling and user interface functions. No private YAML or coordinates/secrets are committed. YAML was parsed structurally; ESPHome compiler validation and the physical-device regression test are still required.

**This eliminates duplicate startup SD work; it does NOT fix the multi-second network stalls.** That requires separately investigating ESPHome synchronous `http_request` / `online_image.set_url` and avoiding increased overlap, with a dedicated async strategy if supported. Current roughly 4-second radar pacing is intentionally preserved because earlier tighter pacing produced TLS errors. Never mask `component:419` warnings by raising thresholds without addressing the cause.

## ATILLA 4.6.5 — defer automatic radar preload after Wi-Fi connection

User requested lower-priority radar downloading. ESPHome 2026.9.1 `online_image::OnlineImage::update()` begins with a synchronous `http_request::get()` call; its later image-body decoding is chunked in `loop()`. A lower FreeRTOS priority is not exposed by the existing YAML `online_image` component and would not fix the synchronous connection phase. Therefore the initial **low-risk experiment** is scheduling rather than changing task priority.

Private full YAML candidate `ATILLA_4.6.5_RADAR_DEFERRED_START_14MHZ_FULL.txt` changes exactly one automatic-start action under `wifi.on_connect` from `delay: 2s` to **`delay: 45s`**, following the existing 5-second wait and weather/forecast script starts. It preserves the `fetch_rainviewer_radar` script, six CARTO map tiles, six RainViewer overlays, the staggered 1.5s/4s pacing, manual RADAR buttons, 14 MHz RGB stability setting, and read-only FATFS mount-only checks. This aims to reduce competition with weather, display initialization, and the ~30-second delayed SD check. It does **not** change FreeRTOS task priority or prevent subsequent blocking requests, and the deferral is measured after Wi-Fi connection rather than absolute boot time.

The private YAML parsed successfully as a full YAML document, but **ESPHome Validate, Compile, OTA, and real startup performance remain unverified**. No private YAML/coordinates/credentials were committed to the public repository. To evaluate, compare `component` blocking warnings, radar startup timestamp, Wi-Fi uptime and image success after install; check manual RADAR access during preload delay separately. Future architectural work may require a dedicated, carefully synchronized off-main-loop HTTP worker.
