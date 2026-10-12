# ATILLA: from SD proof-of-concept to useful graphics memory management

**Architecture review — 2026-10-12. Documentation + small catalog refactor only.**
All publicly submitted changes in this PR are portable. Keep private full-panel YAML,
credentials, coordinates and live URLs outside this repository.

## What we already have — do not reinvent

- CH422G/EXIO4 SD SPI transport with hardware-validated FAT32 mount, capacity
  checks, read-only access and explicit bounded create-new writes.
- ESP-IDF **FatFs** for the filesystem. The hardware SPI chip-select wiring is
  the only board-specific adapter worth retaining; there is no need to build a
  new filesystem or replace it casually with an incompatible GPIO-CS driver.
- ESPHome/ESP-IDF HTTPS and PNGLE, both proven on the actual ESP32-S3 panel.
- LVGL v9 image descriptors, and PSRAM allocations through ESP-IDF. Retained
  descriptors/pixel buffers must outlive the widgets that reference them.
- 4.7.9ad hardware acceptance: first immutable HTTPS download of 96x64 PNG,
  separate reboot verifies `C0003.PNG` unchanged, 26 ms RGBA PNG decode,
  transparent ARGB8888 frame displayed on real panel. No more demo slots.

## What the private 4.7.9ad application actually does

| Retained image | Number | Pixels & encoding | Total live PSRAM |
| --- | ---: | --- | ---: |
| CARTO basemap | 6 | 256x256 RGB565, 131072 bytes each | 786432 bytes / 0.75 MiB |
| RainViewer radar | 6 | 256x256 ARGB8888, 262144 bytes each | 1572864 bytes / 1.5 MiB |
| **Total retained tiled frames** | **12** | | **2359296 bytes / 2.25 MiB** |

Radar refresh also allocates a **temporary 262144-byte decoded frame** and
~43896 bytes for PNGLE; it then copies the finished pixels into each retained
descriptor. This protects the currently displayed frame from failed/partial
decoding. Typical radar PNGLE duration in the supplied Stage B trace was
~245-253 ms per tile, and occasional interval operations exceeded 300 ms.

The private YAML currently starts `fetch_rainviewer_radar` **45 seconds after
Wi-Fi connects**, which triggers map and six radar tile downloads even when
the overview/weather screen is visible. The existing RADAR navigation buttons
independently start that same guarded fetch; a **private compile-only
4.7.9ae candidate** removes just the startup preload, so no tile PSRAM
allocation or tile networking occurs before RADAR is requested. This is the
first immediately useful memory-saving experiment, WITHOUT changing SD code
or releasing LVGL-owned memory. No full-firmware compile or hardware
measurements have yet been run on that candidate.

**Critical limitation:** moving a PNG file to SD does NOT reclaim the decoded
LVGL frame currently on screen. A useful eviction policy must detach the image
source on the LVGL main loop, ensure no LVGL cache reader still holds it, free
the frame, and only later restore it on demand. Never free `descriptor.data`
while the descriptor remains bound to an image widget. Confirm the exact LVGL
cache invalidation API in this ESPHome/LVGL build instead of assuming another
LVGL version's function exists.

## Three concrete functional changes, in order

1. **Lazy initial load (4.7.9ae private compile-only):** remove Wi-Fi-triggered
   preload. Weather, touch, network and SD slot-3 preview continue on boot.
   On first RADAR page entry, existing code fetches all 12 tiles. Acceptance:
   idle boot should retain about 2.25 MiB less map/radar pixel data than
   eager boot, after matching unrelated allocations; all tiles should still
   display upon page entry. This is a conditional, calculated saving — NOT
   a hardware measurement or guaranteed total free-heap delta.
2. **Actual basemap cache:** use existing immutable create-new + CRC/FatFs
   readback for a **real CARTO tile**, with cache identity including style,
   z/x/y, URL/config revision and pixel encoding. Store compressed bytes only
   after successful HTTPS validation; reload from SD on subsequent boots.
   A mismatching/stale asset must be a **cache miss**, not silently reused or
   overwritten. No fixed slot index without verified identity or unbounded
   overwrites. Keep network fallback and per-tile memory budget. No private
   URL or location embedded in a public branch.
3. **Conditional release and reload:** on leaving RADAR, after all pending
   UI handoffs are fenced, detach radar LVGL sources on the main thread,
   invalidate the relevant LVGL cache entries using the actual available API,
   then release up to six decoded radar frames (~1.5 MiB). Re-enter RADAR:
   load the current generation from SD if still valid, otherwise worker HTTPS,
   decode with bounds, bind stable descriptors and show. Do not evict
   basemap until its own page-state lifecycle has been independently proven.
   The current separate temporary frame is retained until a safe atomic
   replacement strategy is validated.

### Tile identities and expiration

- Basemap identity: `{provider, style, zoom, tile_x, tile_y, source_revision,
  decoder/color_format_version}`; may be long-lived while revision matches.
- Radar identity: `{provider, frame_timestamp_or_immutable_frame_path, zoom,
  tile_x, tile_y, color_scheme, filter_revision}`. Dynamic weather data can
  never be keyed solely by slot or coordinates.
- Cache metadata must record exact compressed byte count, CRC, width/height,
  encoding and canonical identity. Missing/invalid metadata implies **MISS**;
  never treat the legacy demo PNG slots as a production tile namespace.
- Versioned asset generations, size quotas, and a bounded cleanup policy are
  required before write-on-refresh radar caching. FAT32 is non-journaled:
  preserve failed staging files for investigation, never overwrite a valid
  committed generation in place, and do not enable automatic deletions yet.

### Threading and ownership

- Network and FatFs disk operations stay on a dedicated worker, never on the
  LVGL/UI task. Only one SD mount owner per operation; all file handles must
  close and the filesystem must unmount before buffer handoff.
- Compressed buffers are **owned** by the worker/handoff and their lifetime is
  deliberately short. Image pixel arrays and descriptors are **owned** by the
  LVGL/main-loop graphics manager. Never pass a temporary byte buffer as
  long-lived `lv_image_dsc_t.data`.
- A cache hit does not mean the decoded image belongs forever in memory.
  Track loading, attached, detaching and evicted states separately, with a
  generation token to reject outdated worker results after navigation.
- Keep a known-good fallback if SD is missing, corrupt, slow or mounted by
  another operation: regular existing worker HTTPS pipeline, without data loss.

## Existing API reduction in this PR

`cache_asset_catalog.h` is the first safe refactor: previously scattered
1/2/3 demo slot sizes, CRCs, dimensions and HTTPS whitelists now live in a
single immutable specification with per-asset PNG verification. The existing
worker and downloader consume it; old pilot modes retain their interface and
log wording. This does not implement production tile caching, page eviction or
a new persistent cache manager. It is intentionally **not** another on-device
write experiment.

## Success metrics for the real optimization

Record free PSRAM/internal heap and largest block at three times:
(a) after boot before RADAR; (b) RADAR fully rendered; (c) after returning to
overview, once a safe detach/eviction implementation exists. Also record first
and repeated RADAR render duration, cache hit/miss generation and SD wear.
A saving is only accepted when pixel buffers were actually released and the
subsequent image reload/display succeeded without stale data or UI regressions.

Never claim that the successful 4.7.9ad fixture, an ESPHome compile, or
passing CI proves reclaimed PSRAM. Validate both visible images and
ownership transitions on hardware before merging experimental PRs.
