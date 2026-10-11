# Repository audit — 2026-10-10

## Scope
Reviewed GitHub file trees for `main`, `dev/display-stability`, `dev/fatfs-readonly`, `dev/fatfs-write-test`, `dev/radar-intensity`, and `dev/storage-service`. Reviewed key main documentation, component code structure and experimental FatFs test design.

## Completed housekeeping
- Corrected the stale main README and release notes using observed hardware results through firmware 4.7.2.
- Deleted the obsolete `docs/CONNECTOR_WRITE_TEST.md` (connector write verification marker).
- Added `.gitignore` for private `waveshare.yaml`, secrets, key files, build outputs and packaged private full-firmware copies.
- Kept the working `main` SD driver unchanged. Experimental code remains segregated from `main`.
- Preserved experimental branches rather than deleting valuable diagnostics and test history.

## Important status distinctions
- `main` has only the conservative, read-only transport and documentation.
- `dev/fatfs-readonly` contains a compiled FatFs diagnostic adapter.
- `dev/fatfs-write-test` has hardware evidence for one 28-byte create/write/verify/delete round trip. It is not production storage.
- `dev/storage-service` contains a candidate 4 KiB multi-sector integrity test and proposed `/ATILLA` directory. Treat as experimental until hardware tested.
- `dev/radar-intensity` contains the precipitation filter verified on a live device. The ambient display brightness target was updated to 25% in its test fixture.
- `dev/display-stability` holds historical display development notes.
- No full private ESPHome device configuration is intended for this public repository.

## Remaining work
1. Compile and hardware-test `dev/storage-service`; investigate failures instead of merging blindly.
2. Reduce long synchronous SD operations on the display's main task, and measure its limited stack headroom.
3. Decide on a noncommercial source-available license. **No license was added during this audit.** License text needs agreement on personal hardware-cost reimbursement and desired sharing rules.
4. Consider archival/branch protection only after confirming no active consumer depends on obsolete experimental code.
5. Periodically scan all branches and commit history for secrets. `.gitignore` only prevents accidental future staging and cannot revoke exposed credentials or scrub git history.
6. Update the component README when promoting an experimental implementation to `main`; it currently documents the conservative read-only path.

## Safety
The user explicitly wants public access for hobby/personal use and to forbid commercial use. Until a custom community noncommercial license is actually published, the repository has no explicit reuse grant. Avoid publishing user-specific full firmware, Wi-Fi credentials, tokens, or home coordinates.
