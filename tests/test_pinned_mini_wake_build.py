"""Offline tests: never fetch from GitHub or invoke real ESPHome."""
import hashlib
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools" / "atilla_lab"))
import pinned_mini_wake_build as mod


class FakeLab:
    def __init__(self, root, *, flash_enabled=False):
        self.workspace = root / "workspace"
        self.workspace.mkdir()
        self.state_dir = root / "state"
        self.state_dir.mkdir(mode=0o700)
        self.project = self.workspace / "mini"
        self.project.mkdir()
        (self.project / mod.CONFIG_NAME).write_text("original firmware", encoding="utf-8")
        self.entry = {"toolchain": "esphome", "flash_enabled": flash_enabled,
                      "config": mod.CONFIG_NAME}

    def _target(self, target):
        if target != mod.TARGET:
            raise KeyError(target)
        return self.entry

    def _project(self, entry):
        return self.project


class MockResponse:
    def __init__(self, content):
        self.content = content

    def __enter__(self):
        return self

    def __exit__(self, *args):
        return False

    def read(self, limit):
        return self.content[:limit]


class PrepareTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.lab = FakeLab(self.root)
        self.source = b"test source from fixed revision\n"
        header = b"blob " + str(len(self.source)).encode("ascii") + b"\0"
        patch_sha1 = patch.object(mod, "SOURCE_BLOB_SHA1",
                                  hashlib.sha1(header + self.source).hexdigest())
        patch_sha256 = patch.object(mod, "SOURCE_SHA256", hashlib.sha256(self.source).hexdigest())
        patch_sha1.start()
        patch_sha256.start()
        self.addCleanup(patch_sha1.stop)
        self.addCleanup(patch_sha256.stop)
        self.calls = []

    def fetcher(self, url, timeout):
        self.assertEqual(url, mod.SOURCE_URL)
        self.assertLessEqual(timeout, 20)
        return MockResponse(self.source)

    def runner(self, argv, **kwargs):
        self.calls.append((argv, kwargs))
        self.assertEqual(argv[:3], [sys.executable, "-m", "esphome"])
        self.assertIn(argv[3], ("config", "compile"))
        self.assertNotIn("shell", kwargs)
        self.assertTrue(kwargs["env"]["ESPHOME_BUILD_PATH"].endswith("cache"))
        return subprocess.CompletedProcess(argv, 0)

    def execute(self, **extra):
        return mod.prepare_and_build(self.lab, fetcher=self.fetcher,
                                     runner=self.runner, **extra)

    def test_good_build_only_never_touches_registered_firmware(self):
        original = (self.lab.project / mod.CONFIG_NAME).read_bytes()
        result = self.execute()
        self.assertTrue(result["build_ok"])
        self.assertFalse(result["flashed"])
        self.assertEqual(result["visual_verification"], "not_performed")
        self.assertEqual((self.lab.project / mod.CONFIG_NAME).read_bytes(), original)
        self.assertEqual((self.lab.project / mod.CANDIDATE_NAME).read_bytes(), self.source)
        self.assertEqual([v[0][3] for v in self.calls], ["config", "compile"])

    def test_idempotent_same_source(self):
        self.execute()
        self.execute()
        self.assertEqual(len(self.calls), 4)

    def test_mismatched_source_fails_before_writes(self):
        with patch.object(mod, "SOURCE_BLOB_SHA1", "0" * 40):
            with self.assertRaisesRegex(mod.PinnedBuildError, "HASH_MISMATCH"):
                self.execute()
        self.assertEqual(self.calls, [])
        self.assertFalse((self.lab.project / mod.CANDIDATE_NAME).exists())

    def test_sha256_mismatch_fails_closed(self):
        with patch.object(mod, "SOURCE_SHA256", "0" * 64):
            with self.assertRaisesRegex(mod.PinnedBuildError, "SHA256_MISMATCH"):
                self.execute()
        self.assertEqual(self.calls, [])

    def test_rejects_other_target(self):
        with self.assertRaisesRegex(mod.PinnedBuildError, "TARGET_NOT_ALLOWLISTED"):
            self.execute(target="waveshare-7inch")
        self.assertFalse(self.calls)

    def test_rejects_enabled_flash_policy(self):
        self.lab.entry["flash_enabled"] = True
        with self.assertRaisesRegex(mod.PinnedBuildError, "TARGET_POLICY_DENIED"):
            self.execute()
        self.assertFalse(self.calls)

    def test_rejects_existing_candidate_conflict(self):
        (self.lab.project / mod.CANDIDATE_NAME).write_text("unrelated source")
        with self.assertRaisesRegex(mod.PinnedBuildError, "CANDIDATE_CONFLICT"):
            self.execute()
        self.assertFalse(self.calls)

    def test_rejects_symlink_candidate(self):
        dest = self.root / "unsafe"
        dest.write_text("unchanged")
        (self.lab.project / mod.CANDIDATE_NAME).symlink_to(dest)
        with self.assertRaisesRegex(mod.PinnedBuildError, "UNSAFE_CANDIDATE_PATH"):
            self.execute()
        self.assertEqual(dest.read_text(), "unchanged")
        self.assertFalse(self.calls)

    def test_validation_failure_stops_compile(self):
        def fail(argv, **kwargs):
            return subprocess.CompletedProcess(argv, 1)
        with self.assertRaisesRegex(mod.PinnedBuildError, "VALIDATE_FAILED"):
            mod.prepare_and_build(self.lab, fetcher=self.fetcher, runner=fail)
        self.assertFalse((self.lab.state_dir / "pinned-mini-wake-build" / "build.log").exists())

    def test_build_failure_not_reported_as_pass(self):
        def fail_compile(argv, **kwargs):
            return subprocess.CompletedProcess(argv, 1 if argv[3] == "compile" else 0)
        with self.assertRaisesRegex(mod.PinnedBuildError, "BUILD_FAILED"):
            mod.prepare_and_build(self.lab, fetcher=self.fetcher, runner=fail_compile)

    def test_source_size_bound(self):
        with self.assertRaisesRegex(mod.PinnedBuildError, "SIZE_INVALID"):
            mod.prepare_and_build(self.lab, fetcher=lambda url, timeout: MockResponse(b"a" * (mod.MAX_BYTES + 1)), runner=self.runner)


if __name__ == "__main__":
    unittest.main()
