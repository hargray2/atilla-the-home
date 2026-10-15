"""Proposed ATILLA-LAB *build-only* operation for one pinned CYD Mini revision.

Integration is intentionally NOT live. The registered ESPHome YAML, firmware,
flashing policy, credentials and other ESP targets remain unchanged.
"""
from __future__ import annotations

import hashlib
import os
from pathlib import Path
import subprocess
import sys
from urllib.request import urlopen

TARGET = "atilla-mini"
CONFIG_NAME = "atilla-mini-1.5.7-idle-display.yaml"
CANDIDATE_NAME = "atilla-mini-wake-candidate.yaml"
SOURCE_COMMIT = "1d6af8db41cfd379a63391e15642e36675c84e0a"
SOURCE_BLOB_SHA1 = "1de37491275d0838a8081053d7f9fa057ce57223"
SOURCE_SHA256 = "cf2feec03246bd84496ad7eb0b3f8273e6e68e50972c942edf03e6c99edc7503"
SOURCE_URL = (
    "https://raw.githubusercontent.com/hargray2/atilla-the-home/"
    + SOURCE_COMMIT + "/experimental/atilla-mini/" + CONFIG_NAME
)
MAX_BYTES = 150_000


class PinnedBuildError(RuntimeError):
    """No raw stdout, secrets or internal filesystem details in the error."""


def _read_pinned_source(fetcher):
    try:
        # Fetcher has the urllib.request.urlopen shape; can be stubbed in tests.
        with fetcher(SOURCE_URL, timeout=20) as response:
            data = response.read(MAX_BYTES + 1)
    except Exception as exc:
        raise PinnedBuildError("PINNED_SOURCE_FETCH_FAILED") from exc
    if not isinstance(data, bytes) or not 0 < len(data) <= MAX_BYTES:
        raise PinnedBuildError("PINNED_SOURCE_SIZE_INVALID")
    header = b"blob " + str(len(data)).encode("ascii") + b"\x00"
    if hashlib.sha1(header + data).hexdigest() != SOURCE_BLOB_SHA1:
        raise PinnedBuildError("PINNED_SOURCE_HASH_MISMATCH")
    if hashlib.sha256(data).hexdigest() != SOURCE_SHA256:
        raise PinnedBuildError("PINNED_SOURCE_SHA256_MISMATCH")
    return data


def _secure_dir(path: Path, parent: Path) -> Path:
    if path.is_symlink():
        raise PinnedBuildError("UNSAFE_BUILD_DIRECTORY")
    path.mkdir(mode=0o700, parents=True, exist_ok=True)
    resolved = path.resolve(strict=True)
    if not resolved.is_relative_to(parent.resolve(strict=True)):
        raise PinnedBuildError("UNSAFE_BUILD_DIRECTORY")
    if path.stat().st_mode & 0o077:
        raise PinnedBuildError("BUILD_DIRECTORY_PERMISSIONS_UNSAFE")
    return resolved


def prepare_and_build(lab, *, target: str = TARGET, fetcher=urlopen,
                      runner=subprocess.run, python_executable: str = sys.executable):
    """Prepare, validate, compile. No flash/upload/OTA/erase calls are reachable."""
    if type(target) is not str or target != TARGET:
        raise PinnedBuildError("TARGET_NOT_ALLOWLISTED")
    try:
        entry = lab._target(target)
        if entry.get("toolchain") != "esphome" or entry.get("flash_enabled") is not False:
            raise PinnedBuildError("TARGET_POLICY_DENIED")
        if entry.get("config") != CONFIG_NAME:
            raise PinnedBuildError("CONFIG_NOT_PINNED")
        project_arg = Path(lab._project(entry))
        workspace = Path(lab.workspace).resolve(strict=True)
        state_root = Path(lab.state_dir).resolve(strict=True)
        if project_arg.is_symlink():
            raise PinnedBuildError("UNSAFE_REGISTERED_PROJECT")
        project = project_arg.resolve(strict=True)
        if not project.is_dir() or not project.is_relative_to(workspace):
            raise PinnedBuildError("UNSAFE_REGISTERED_PROJECT")
        registered_path = project / CONFIG_NAME
        if registered_path.is_symlink() or not registered_path.is_file():
            raise PinnedBuildError("REGISTERED_CONFIG_UNAVAILABLE")
    except PinnedBuildError:
        raise
    except Exception as exc:
        raise PinnedBuildError("TARGET_PREFLIGHT_FAILED") from exc

    source = _read_pinned_source(fetcher)
    source_sha256 = hashlib.sha256(source).hexdigest()
    candidate = project / CANDIDATE_NAME
    if candidate.is_symlink():
        raise PinnedBuildError("UNSAFE_CANDIDATE_PATH")
    if candidate.exists():
        if not candidate.is_file() or candidate.read_bytes() != source:
            raise PinnedBuildError("CANDIDATE_CONFLICT")
        if candidate.stat().st_mode & 0o077:
            raise PinnedBuildError("CANDIDATE_PERMISSIONS_UNSAFE")
    else:
        flags = os.O_CREAT | os.O_EXCL | os.O_WRONLY
        if hasattr(os, "O_NOFOLLOW"):
            flags |= os.O_NOFOLLOW
        try:
            fd = os.open(candidate, flags, 0o600)
            with os.fdopen(fd, "wb") as output:
                output.write(source)
        except FileExistsError as exc:
            raise PinnedBuildError("CANDIDATE_CONFLICT") from exc
        except OSError as exc:
            raise PinnedBuildError("CANDIDATE_STAGE_FAILED") from exc
    if candidate.read_bytes() != source:
        raise PinnedBuildError("CANDIDATE_HASH_MISMATCH")

    # A private evidence/cache directory, distinct from the registered config.
    evidence_dir = _secure_dir(state_root / "pinned-mini-wake-build", state_root)
    cache = _secure_dir(evidence_dir / "cache", state_root)
    env = os.environ.copy()
    env["ESPHOME_BUILD_PATH"] = str(cache)
    # Fixed command list; cannot be overridden by the model or by the URL.
    commands = (
        ("validate", [python_executable, "-m", "esphome", "config", CANDIDATE_NAME], 180),
        ("build", [python_executable, "-m", "esphome", "compile", CANDIDATE_NAME], 1200),
    )
    for stage, argv, timeout in commands:
        logpath = evidence_dir / (stage + ".log")
        if logpath.is_symlink():
            raise PinnedBuildError("UNSAFE_LOG_PATH")
        fd = os.open(logpath, os.O_CREAT | os.O_WRONLY | os.O_TRUNC | getattr(os, "O_NOFOLLOW", 0), 0o600)
        with os.fdopen(fd, "wb") as log:
            try:
                completed = runner(argv, cwd=str(project), env=env, timeout=timeout,
                                   stdin=subprocess.DEVNULL, stdout=log,
                                   stderr=subprocess.STDOUT, check=False)
            except subprocess.TimeoutExpired as exc:
                raise PinnedBuildError(stage.upper() + "_TIMEOUT") from exc
            except Exception as exc:
                raise PinnedBuildError(stage.upper() + "_EXECUTION_FAILED") from exc
        if completed.returncode != 0:
            raise PinnedBuildError(stage.upper() + "_FAILED")

    # Immutable source provenance; does not establish firmware installation or UI acceptance.
    return {"ok": True, "target": TARGET, "stage": "build",
            "config_sha256": source_sha256, "source_blob": SOURCE_BLOB_SHA1,
            "source_commit": SOURCE_COMMIT, "build_ok": True,
            "flashed": False, "visual_verification": "not_performed"}
