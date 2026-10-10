# Radar overlay intensity filter — prototype (not installed)

**User-facing design:** Keep the basemap clearly visible. Remove very weak diffuse returns altogether, then color retained precipitation **green → yellow → red → purple**, with semi-transparent fills, so coastlines, roads, city names and the HOME marker remain legible.

**Data source:** RainViewer now provides only Universal Blue (scheme 2), so custom coloring happens **locally after decoding**, not by requesting another color scheme. The experimental C++ helper `experimental/radar_intensity_filter.h` uses an explicit RainViewer 0–64 dBZ color lookup from the [official color table](https://www.rainviewer.com/files/rainviewer_api_colors_table.csv). It changes only radar tiles; never touch the CARTO basemap buffer.

- Under 30 dBZ: alpha = 0 (invisible).
- 30–34 dBZ: green, output alpha at most ~56% of source.
- 35–44 dBZ: yellow, output alpha at most ~78%.
- 45–54 dBZ: red, output alpha at most ~78%.
- 55+ dBZ: purple, output alpha at most ~78%.

The current prototype is **not integrated into the online_image finished-download callbacks, has not compiled in ESPHome CI, and has not been hardware tested**. It is NOT production ready. This in-place transformation requires the radar tile image to be `transparency: alpha_channel` (not chroma key), have a known byte layout, and be complete before filtering. The callbacks should process each tile exactly once per new decode, never repeatedly reduce alpha on an already-filtered cached frame.

**Important**: The existing RainViewer URLs end in `/2/1_1.png`, which enables smoothing. RainViewer interpolates palette colors for smooth edges; an exact color lookup won't recognize all those intermediate colors. The first controlled trial should use the same official Universal Blue scheme but **unsmoothed radar tiles** (`/2/0_1.png`) when supported and confirm all desired precipitation returns map correctly. If retaining smooth=1 is necessary, implement a nearest-palette match with bounded color-distance safeguards and test it on real downloaded PNG files. Unrecognized colors currently remain unchanged; this is deliberately safer than erasing legitimate radar data but can leave some blue until validation is complete.

To complete integration: add a small ESPHome external component that includes the header, exposes a one-shot action called from the radar online-image `on_download_finished` handler, and invalidates/redraws the tile once filtered. Avoid touching LVGL buffers in worker threads. Add compile-only smoke test that uses fake GPIO-free IDs and checks the target ESPHome 2026.9.1 API; then install only via a private complete YAML generated from the latest actual source. Do not commit users' private map key or coordinates.

Do not conflate the no-op FatFs entry probe with this separate image filter branch.

## 2026-10-10 — first ESPHome action integration

A new opt-in ESPHome external component now lives in `components/atilla_radar_filter` with an `atilla_radar_filter.apply` automation action. An isolated ESPHome 2026.9.1 smoke test and GitHub Actions workflow are included. **This is a separate development branch, not the user's complete private firmware.**

For each of the six real RainViewer `online_image` entities, integration must use RGB / alpha channel and call the action only on fresh downloads:

```yaml
on_download_finished:
  then:
    - if:
        condition:
          lambda: "return !cached;"
        then:
          - atilla_radar_filter.apply:
              id: rain_filter
              image_id: radar_tile_0  # substitute each actual image id
```

The integration must configure exactly one `atilla_radar_filter:` instance with `id: rain_filter`. Use **unsmoothed** Universal Blue RainViewer request `.../2/0_1.png` rather than the current `.../2/1_1.png`, unless the palette matcher is extended to handle smoothing. Apply strictly to radar overlay images, never base-map tiles. Assert RGB/alpha-channel images are decoded to the expected BGRA layout in the actual ESPHome build and verify that LVGL refreshes after alpha changes; passing compilation cannot establish correct appearance. Multiple display/image buffers require testing for rendering cache invalidation.

Currently the action passes `alpha_channel_configured=true` internally; this must be enforced via the full private YAML when integrated, because `has_transparency()` alone does not distinguish alpha channel from chroma key. **Do not integrate into an unknown YAML image type.** Cached 304 images must not be filtered again (which could compound alpha). Color-matching uses the official discrete palette; unsmoothed server tiles are necessary for exact lookup. Keep the map readable and do not erase unrecognized colors until validated.

Automated CI: https://github.com/hargray2/atilla-the-home/actions?query=branch%3Adev%2Fradar-intensity (currently pending).
