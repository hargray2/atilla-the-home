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
| **4.7.9t** | **Hardware-validated checkpoint:** both weather workers PASS with LIVE / UPDATED, six CARTO basemap tiles, six RainViewer radar tiles, read-only FatFs batch PASS; ~4,895 kB PSRAM and ~100 kB internal heap free after startup |

The 4.7.9t acceptance record is in [VALIDATED-BASELINE-4.7.9t.md](VALIDATED-BASELINE-4.7.9t.md). The complete device YAML remains private. Runtime logs establish early-operation success, not an extended reliability soak.
