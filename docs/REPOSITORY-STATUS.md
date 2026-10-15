# Repository status and review map

*Verified from GitHub on October 15, 2026. Update this page when dependencies or hardware evidence change.*

## What to start with

- **[ATILLA ecosystem overview](../README.md):** the public entry point for all three projects.
- **[Waveshare 7-inch](ecosystem/WAVESHARE-7-INCH.md):** public storage/radar components, private complete display firmware and dated hardware acceptance.
- **[ATILLA Mini](ecosystem/ATILLA-MINI.md):** separate CYD firmware history on [`dev/cyd-mini-bringup`](https://github.com/hargray2/atilla-the-home/tree/dev/cyd-mini-bringup).
- **[ATILLA-LAB](ecosystem/ATILLA-LAB.md):** installed Debian firmware-development station, documented here but maintained outside this public repository.
- **[Release notes](RELEASE-NOTES.md):** historical hardware milestones; a successful smoke fixture is not a full firmware release.
- **[Earlier audit and inventory](https://github.com/hargray2/atilla-the-home/tree/dev/cyd-mini-bringup/docs):** preserved dated audit material and full tracked-file inventory from the October 15 documentation review.

## Branch reality

`main` contains the public Waveshare components and documentation. `dev/cyd-mini-bringup` contains Mini YAML and related acceptance records. The server's active `atilla_control_server_v0_3` checkout is not public here. Do **not** merge these branches wholesale merely to make the homepage complete; they contain distinct component revisions and hardware-sensitive logic.

## Open SD-cache pull requests: deliberate experimental chain

These are **draft work**, not five unrelated approval buttons. Branches remain available for comparison and future review.

| Order | Draft PR | Base branch | Evidence / gate |
|---|---|---|---|
| 1 | [#9 — pinned HTTPS-to-SD pilot](https://github.com/hargray2/atilla-the-home/pull/9) | `main` | Private device logs report the 70-byte slot-2 pilot passed; separate latest-head source, CI and safety review remain before promotion |
| 2 | [#10 — bounded cache reader](https://github.com/hargray2/atilla-the-home/pull/10) | PR #9's head | Read-only PSRAM handoff; compilation fixtures are not full panel hardware acceptance |
| 3 | [#11 — LVGL blue-square preview](https://github.com/hargray2/atilla-the-home/pull/11) | PR #10's head | Private on-device preview reported passing for fixed 1×1 PNG; no generalized renderer |
| 4 | [#12 — multicolor slot-3 pilot](https://github.com/hargray2/atilla-the-home/pull/12) | PR #11's head | Small static test image; not yet validated on physical hardware |
| 5 | [#13 — shared asset catalog / tile-memory design](https://github.com/hargray2/atilla-the-home/pull/13) | PR #12's head | Shared experimental cache definitions; dynamic tile eviction and promised memory savings not yet hardware-proven |

**Do not merge all drafts.** If experimental storage is revisited, work from the bottom dependency forward, confirm exact source revisions, check every current test and preserve the known-good rollback. Never enable SD writes, OTA uploads or appliance control as a side effect of merging docs.

The older [#1 documentation audit](https://github.com/hargray2/atilla-the-home/pull/1) was **closed without merging** because its README and engineering claims were superseded by the October 15 ecosystem update. Its historical branch remains available.

## Public repository housekeeping

- **About:** accurately describes Waveshare, Mini, Home Assistant and ATILLA-LAB.
- **README:** public ecosystem landing page, with project links and a labelled illustration (not a real-device photograph).
- **Topics:** maintainer GitHub metadata still needs `esphome`, `esp32`, `home-assistant`, `lvgl`, `waveshare`, `cyd`, `firmware`, and `iot`. Topics are edited using the gear beside the repository About section; this connector cannot set them.
- **Licensing:** no license grant has been selected. A public repository is readable, but not automatically open source; the owner's noncommercial/reuse requirements must be resolved before publishing license text.
- **Visual evidence:** use images of *actual* hardware and firmware only when privacy-reviewed. Never publish EXIF location data, a private home map center, Wi-Fi credentials, real device identities, API tokens or photographs showing other people.

See the [security and contribution guide](SECURITY-AND-CONTRIBUTING.md) and [release workflow](RELEASE-WORKFLOW.md).
