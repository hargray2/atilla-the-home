#!/usr/bin/env bash
# ATILLA Mini remote-wake candidate: pinned source, local validate + build ONLY.
# Never flash, erase, upload, install, run, or call Home Assistant services.
set -euo pipefail
umask 077

PIN_COMMIT="1d6af8db41cfd379a63391e15642e36675c84e0a"
PIN_BLOB="1de37491275d0838a8081053d7f9fa057ce57223"
YAML_REL="experimental/atilla-mini/atilla-mini-1.5.7-idle-display.yaml"
CANDIDATE_NAME="atilla-mini-wake-candidate.yaml"

REPO_ROOT="$(git -C "$(dirname "$0")" rev-parse --show-toplevel)"
TARGET_DIR="$REPO_ROOT/experimental/atilla-mini"
CANDIDATE="$TARGET_DIR/$CANDIDATE_NAME"
LOG_DIR="${ATILLA_WAKE_LOG_DIR:-$HOME/.cache/atilla-lab/mini-wake}"
mkdir -p "$LOG_DIR"
chmod 700 "$LOG_DIR"
# Do not overwrite the registered target's normal .esphome/build/atilla-mini cache.
export ESPHOME_BUILD_PATH="$LOG_DIR/build"
mkdir -p "$ESPHOME_BUILD_PATH"
chmod 700 "$ESPHOME_BUILD_PATH"

if ! git -C "$REPO_ROOT" cat-file -e "$PIN_COMMIT^{commit}" 2>/dev/null; then
  echo "Pinned source commit is absent: fetching the wake proposal branch." >&2
  git -C "$REPO_ROOT" fetch --no-tags origin dev/mini-remote-wake-proposal
fi
if ! git -C "$REPO_ROOT" cat-file -e "$PIN_COMMIT^{commit}" 2>/dev/null; then
  echo "ERROR: pinned revision unavailable. No candidate generated." >&2
  exit 1
fi

TMP="$(mktemp "$TARGET_DIR/.atilla-wake-XXXXXX")"
trap 'rm -f "$TMP"' EXIT
git -C "$REPO_ROOT" show "$PIN_COMMIT:$YAML_REL" > "$TMP"
BLOB="$(git -C "$REPO_ROOT" hash-object "$TMP")"
if [[ "$BLOB" != "$PIN_BLOB" ]]; then
  echo "ERROR: pinned YAML does not match reviewed Git blob. Stopping." >&2
  exit 1
fi
mv -f -- "$TMP" "$CANDIDATE"
trap - EXIT
echo "PINNED SOURCE VERIFIED: $PIN_COMMIT"
echo "CANDIDATE YAML: $CANDIDATE_NAME"
sha256sum "$CANDIDATE"

# Build using the Dell's local toolchain if available; CI uses its pinned ESPHome.
LAB_PY="$HOME/atilla_control_server_v0_3/.venv/bin/python"
if [[ -x "$LAB_PY" ]]; then
  ESPHOME=("$LAB_PY" -m esphome)
elif command -v esphome >/dev/null 2>&1; then
  ESPHOME=(esphome)
else
  echo "ERROR: No ESPHome toolchain detected. Candidate is prepared only." >&2
  exit 1
fi

cd "$TARGET_DIR"
if ! "${ESPHOME[@]}" config "$CANDIDATE_NAME" > "$LOG_DIR/validate.log" 2>&1; then
  echo "VALIDATION FAILED. Inspect $LOG_DIR/validate.log locally." >&2
  exit 1
fi
echo "VALIDATION PASSED"
if ! "${ESPHOME[@]}" compile "$CANDIDATE_NAME" > "$LOG_DIR/compile.log" 2>&1; then
  echo "BUILD FAILED. Inspect $LOG_DIR/compile.log locally." >&2
  exit 1
fi
echo "BUILD PASSED (no device writes)"
echo "Saved private build logs: $LOG_DIR"
