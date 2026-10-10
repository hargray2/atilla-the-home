# ATILLA architecture

## Working firmware baseline: ATILLA 4.5

Installed via Home Assistant ESPHome Builder from a **single** `waveshare.yaml` file, maintained on an iPhone. The ESPHome node retains the filename `waveshare.yaml`; the dashboard/project name is **ATILLA THE HOME**.

### UI and map

- ESP32-S3 drives LVGL on the 800×480 touch panel.
- HOME view, RADAR view, on-screen zoom +/- and CENTER controls.
- Map consists of six 256×256 CARTO dark basemap tiles, at zoom level 6, x=16–18 and y=25–26 in the historically tested arrangement.
- RainViewer radar overlays consist of six corresponding transparent tile images.
- Each of the 12 images is fetched using ESPHome `online_image`.
- `on_download_finished` triggers `lvgl.image.update` so completed transfers become visible.
- The map and overlays share zoom and pan transformations.
- Radar tiles are preloaded while on HOME, allowing the radar view to open promptly once downloads finish.

### Why transfers are staggered

Historical working pacing: ~1.5 seconds between basemap requests, ~4 seconds between radar requests. Faster bursts produced TLS handshake/connect errors, including mbedTLS resource failures. Avoid changing pacing without benchmark evidence.

### RAM versus persistence

Image tiles currently reside in RAM for the active boot. A reboot requires network retrieval. FAT32 sector-reading success is **not equivalent** to mounted FATFS or saved images.

## Planned external component path

```text
waveshare.yaml  (private, one-file installation)
      |
      +-- external_components: GitHub atilla_storage
                    |
                    +-- ESPHome codegen Python
                    +-- C++ transport (CH422G CS)
                    +-- future read-only FATFS disk adapter
                    +-- future guarded write implementation
                    +-- future asset cache / LVGL image integration
```

The transport is experimental and is **not activated for card access** by the current component's setup method. The external component has experienced Python type and C++ namespace compilation errors; their fixes are committed, but a successful subsequent build has not yet been reported.

## Privacy architecture

The complete production YAML can contain Wi-Fi, API and map credentials. Keep it off this **public** repository. Reusable driver code, sanitized examples, logs with sensitive strings removed, and documentation belong here.
