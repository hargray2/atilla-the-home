# ATILLA repository audit — 2026-10-15

> **Review scope, not a release or a hardware acceptance.** This report accompanies a documentation-and-test review branch based on `docs/atilla-ecosystem-20261014`. No private firmware was published, no Dell source was modified, and no ESP was flashed.

## Source coverage and method

- Connected GitHub repository: `hargray2/atilla-the-home`; inspected the default `main`, Mini `dev/cyd-mini-bringup`, `docs/atilla-ecosystem-20261014`, previous audit/overhaul refs, and 19 historical development branch HEAD trees. All 24 listed branch heads were inventoried; branch history and every commit are **not** audited.
- Full Git tree and file contents inspected for **101 files** (approximately **548 KiB**) in the ecosystem-doc branch, covering workflows, `.gitignore`, public C++/Python components, all carried Mini YAML snapshots, docs, fixtures, assets and tools. The [file inventory](FILE-INVENTORY-2026-10-15.md) records the paths and their role.
- Main branch inventory: **67 files** (~248 KiB). Mini development branch: **94 files** (~500 KiB). Selected main hardware evidence and component/release documentation were inspected separately; a full byte-by-byte security review of every divergent branch is not claimed.
- Markdown relative-link targets were checked against the inspected ecosystem branch tree; no unresolved local link path was identified in that snapshot. External URLs, heading anchors, historical git objects and GitHub Actions results require independent checks.
- Static review only for most C++/ESPHome code: no compilation of *all* branch HEADs or physical Waveshare/Mini hardware requalification occurred during this repository audit.

## Confirmed findings

| Priority | Finding | Evidence and disposition |
|---|---|---|
| High | `main` represents Waveshare-focused components, not the full ATILLA ecosystem | Default README still describes only the 7-inch board. Ecosystem docs are in unmerged [PR #14](https://github.com/hargray2/atilla-the-home/pull/14); do not imply the default repository already exposes them |
| High | Waveshare hardware milestones diverged by branch | Ecosystem docs carried **4.7.9y** while `main` documents **4.7.9z** read-only manifest tests and **4.7.9aa** limited immutable-cache write/persistence. A successful 70-byte write pilot is **not** a generic SD write service |
| High | Mini's project landing page described only 0.1.0 first-flash bring-up | Mini branch contains versioned firmware through **1.5.7**. Updated its README to serve as a version/build/acceptance index and removed automatic flash instructions from the current entry workflow |
| High | LAB overview lagged behind installed capabilities | On 2026-10-15 ChatGPT successfully queued native ATILLA-Mini `validate` and `build` jobs through authenticated timed access; installed firmware writes remained disabled. Updated LAB docs to distinguish verified behavior, proposed features, source identity, and permissions |
| Medium | Exact source identity absent from remote firmware result | Native build succeeded for registered target, but API did not report the YAML path. A passed build does not establish the intended 1.5.7 file was built; capture exact source, build digest, bounded diagnostic logs before release |
| Medium | Historical notes can be misread as current release status | Retained timestamped milestones and added current navigation. Historical source and acceptance reports are not redundant merely because superseded by newer experiments |
| Medium | Testing/evidence coverage is limited | CI fixtures test components, not the private complete `waveshare.yaml`, all real board wiring, or a safe physical flash. Existing CI actions use version tags (`actions/checkout@v4`, `setup-python@v5`) rather than immutable SHAs |
| High | GitHub repository About metadata is Waveshare-only | The actual description remains `Development files for Atilla on Waveshare esp32-s3-touch-lcd-7` with no topics, despite three distinct current projects. Suggested About: **ATILLA ESP development ecosystem: Waveshare 7-inch and CYD Mini dashboards, ESPHome components, hardware tests, and a separate Debian automation lab.** Suggested topics: `esphome`, `esp32`, `home-assistant`, `lvgl`, `waveshare`, `cyd`, `embedded`, `firmware`. The available connector cannot update About metadata; repository owner should apply this in GitHub Settings before publication |\n| Medium | No published `LICENSE` file in inspected tree | The 2026-10-10 audit records an unresolved noncommercial licensing preference. Do **not** add a license or claim an open-source reuse grant without an explicit licensing decision |
| Medium | Separate ATILLA-LAB checkout is absent from this repo | Public firmware components, docs and Mini snapshots are here. Dell control server's live files and private credentials remain separate; a genuine audit of the server needs its own source checkout and tests |
| Low | Previous obsolete connector test marker | `docs/CONNECTOR_WRITE_TEST.md` is absent from the existing ecosystem PR branch as noted in the October 14 audit. Do not delete real fixtures to simulate cleanup |

## Changes on this review branch

1. Refreshed `experimental/atilla-mini/README.md` with the 1.5.7 source index, prior snapshots, build-only commands, secrets boundary and hardware gates.
2. Updated `docs/ecosystem/ATILLA-LAB.md` and `docs/ecosystem/ATILLA-MINI.md` to separate verified remote build operations from outstanding configuration-identity/logging and hardware acceptance.
3. Updated `docs/ecosystem/WAVESHARE-7-INCH.md` to document the 4.7.9y versus 4.7.9z/aa branch divergence and limited SD-write evidence.
4. Added root README links to review status, this audit, and Mini/LAB subproject entry points.
5. Added `tools/check_repository_docs.py`, offline unit tests, and `.github/workflows/repository-docs.yml` to catch missing/local path escapes in Markdown links on future changes. The script does **not** execute remote URLs or modify the repository.
6. Added the dated file-by-file inventory to make preservation decisions reviewable.

## Preserved; do not delete without a dependency check

- All versioned Mini YAML files, because each captures a different firmware/hardware checkpoint and there is no proof that none are used for regression or rollback.
- Waveshare `atilla_storage`/`atilla_radar_filter` code, SD write-gated routines and test fixtures. Unused *in one branch* does not mean safe to delete from an active external component referenced by private firmware.
- Previous audit reports, acceptance logs, low-level experiments and older development branches. These are provenance records. Archive only after verifying branch references, PR dependencies and hardware rollback use.
- Donor/support assets already included in the existing unmerged documentation PR; publication of any personal contact/payment information remains subject to the repository owner's review.

## Required release/merge gates

1. Resolve [PR #14](https://github.com/hargray2/atilla-the-home/pull/14) and branch precedence explicitly; do not merge the old Mini branch's storage component over newer `main` without assessing changed driver flags and behaviors.
2. Confirm this review branch's Markdown-link CI and tests pass in GitHub; then review the complete diff and integrate into the ecosystem documentation PR.
3. Before another Waveshare firmware deployment, compare the private full YAML, its pinned component commits, latest accepted hardware test and rollback image. Never infer runtime acceptance from a smoke-fixture build.
4. Add a separate, authorized history-wide secret scan and third-party license review before general publication. `.gitignore` is not a secret-history scrub.
5. Add exact YAML source identity, bounded redacted compiler diagnostics and eventual camera/serial acceptance to ATILLA-LAB's remote build reports. Keep hardware-write authorization disabled by default.
6. Obtain an explicit licensing decision before adding a reuse license. No licensing terms were changed in this audit.

**Cleanup conclusion:** No additional production code or historical file met a defensible safe-delete threshold from the reviewed evidence. The audit prioritizes accurate status, navigability, tests and preserving working rollback paths over speculative deletion.
