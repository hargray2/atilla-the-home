# Repository audit — 2026-10-14

## Scope and evidence
Audited the visible root directories, the documentation index, component boundaries, Mini hardware acceptance, Waveshare 4.7.9y hardware acceptance, previous 2026-10-10 audit, repository checklist, security guidance and `.gitignore` on `dev/cyd-mini-bringup`. This is a **focused repository-structure/documentation audit**, not a full source-security scan, dependency audit, or execution of device tests.

## Applied changes (review branch)
- Added `docs/ecosystem/` with separate entry points for Waveshare 7-inch, Mini, ATILLA-LAB, and integrations.
- Updated root README to link the ecosystem and use the newest *documented* Waveshare acceptance on the inspected branch (4.7.9y rather than 4.7.9u). No implication that the complete private firmware is public.
- Removed only `docs/CONNECTOR_WRITE_TEST.md`: its entire contents were an obsolete GitHub connector write-access marker, contradicting the prior audit's removal claim. No firmware, test fixture or engineering history was removed.
- Clarified the web-first Home Assistant IP/token Connect & Discover experience as the server-side setup direction; device flashing and erase rules remain independent.

## Preserved deliberately
- `components/` production and experimental firmware code, `tests/` smoke fixtures, `release/` lockfile and historical `docs/` acceptance reports.
- The private `waveshare.yaml` is still excluded by `.gitignore`; the current main branch is not automatically the latest hardware baseline.
- `experimental/atilla-mini/` remains a distinct Mini source, not a replacement for the Waveshare firmware.
- ATILLA-LAB lives in a separate checkout and local server installation; its generated archives and API credentials should **not** be copied into this public firmware repository.

## Remaining issues to tackle separately
1. The historic `docs/REPOSITORY-CHECKLIST.md` and some experimental READMEs are snapshot documents with older checkpoints. Preserve them as dated records, then build a current release-status index rather than silently rewriting history.
2. Full Waveshare firmware reproducibility remains impossible from public source alone. Keep private build and rollback material protected.
3. For the ESP flashing automation, run negative authorization/replay/port-swap/recovery tests on the Dell prior to enabling any hardware write.
4. Verify CI branch status, repository-wide git history and secrets exposure with appropriate tools before merge or new release.
5. Audit third-party licenses and attribution independently. No licensing grant was changed in this patch.

## Test status
Documentation review only. GitHub accepted the file edits. **No ESPHome compile, firmware flash, erase, Home Assistant write or hardware acceptance was performed as part of this audit.**
