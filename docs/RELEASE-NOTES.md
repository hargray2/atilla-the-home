# ATILLA release notes (v4 onward)

This log distinguishes hardware-observed behavior from experimental proposals. Versions before v4 are intentionally excluded.

| Version | Evidence / status |
|---|---|
| 4.0.3 | Experimental large root-directory scan; OTA rollback observed |
| 4.0.4 | Conservative one-sector delayed SD diagnostic passed |
| 4.0.5–4.2 | Read-only FAT32 and file-CRC diagnostics advanced |
| 4.2.1 | Reduced SD diagnostic delay and observed success |
| 4.3 | Added map and radar request pacing |
| 4.4 | Added internal heap and PSRAM diagnostics |
| 4.5 | Known-good earlier LVGL/map/radar dashboard |
| 4.6–4.7 | External FatFs component and guarded storage experiments; not a production storage service |
| 4.7.9p | Hardware: read-only FatFs mount, known-file CRC and five-phase batch read PASS; 666 ms main-loop warning and only 686 bytes of observed stack headroom |
| 4.7.9q | Hardware: moved batch probe to 24,576-byte FreeRTOS worker; ~17,428 bytes minimum stack free; main-loop storage warning absent |
| 4.7.9r | Hardware: weather networking moved into workers; weather TLS/validation FAILED; other subsystems passed |
| 4.7.9s | Hardware: serialized HTTPS; weather received 200 bodies but FAIL due to response-length handling |
| **4.7.9u** | **Latest hardware-validated startup checkpoint:** consolidated SHA-pinned storage+radar components, weather current+forecast, all 6 CARTO and radar tiles, read-only FatFs batch PASS; see advisories below |
| 4.7.9t | **Hardware-validated checkpoint:** both weather workers PASS with LIVE / UPDATED, six CARTO basemap tiles, six RainViewer radar tiles, read-only FatFs batch PASS; ~4,895 kB PSRAM and ~100 kB internal heap free after startup |

## Release advisories — 4.7.9u

- **Validation scope:** The observed device log confirms compilation, boot and core subsystem startup over roughly two minutes, **not** long-duration stability, all touchscreen controls, offline recovery or sustained radar refresh cycles.
- **API reconnect:** ESPHome's initial API attempt reported `SocketClosedAPIError` / EOF. A subsequent connection and encrypted handshake succeeded. The cause was not established; monitor for recurring connection loss.
- **Main-loop latency:** CARTO and radar image processing still produced component-operation warnings of approximately **181–493 ms**. These are not the previously fixed multi-second synchronous weather HTTPS stalls. LVGL responsiveness and repeated refreshes still warrant testing.
- **Memory:** After tile loading, approximately **4,895 kB PSRAM** and **100 kB internal heap** remained free; the largest internal heap block varied (31–53 kB). Values are observations, not guaranteed headroom across long runtimes.
- **microSD safety:** The verified FatFs path is **read-only** (mount, 12-byte CRC check, root directory scan and unmount). It is **not** general-purpose SD asset loading; no production writes, automatic formatting or power-loss recovery are validated.
- **External components:** The 4.7.9u firmware uses two components from pinned Git commit `5581459f87753977151f43f7a853f3604efa1b84`. Merging this repository branch into `main` **does not automatically change** that deployed immutable source reference or the private firmware.
- **Rollback:** Keep the full private 4.7.9u firmware and its pinned component commit; retain 4.7.9t as the previous hardware-tested fallback. Do not assume an isolated GitHub Actions compile proves a full-panel OTA build.

See [4.7.9u acceptance](VALIDATED-BASELINE-4.7.9u.md) and [4.7.9t acceptance](VALIDATED-BASELINE-4.7.9t.md). The complete device YAML remains private. Release history intentionally starts at v4.
