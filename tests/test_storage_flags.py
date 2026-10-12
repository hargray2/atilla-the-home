"""Regressions for FatFs mode validation and worker dispatch (stdlib only)."""
import ast
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CONFIG = ROOT / "components/atilla_storage/__init__.py"
HEADER = ROOT / "components/atilla_storage/atilla_storage.h"
FIXTURE = ROOT / "tests/fatfs-smoke.yaml"


class FakeCV:
    class Invalid(ValueError):
        pass


def load_validator():
    tree = ast.parse(CONFIG.read_text(encoding="utf-8"))
    constants = {
        item.targets[0].id: item.value.value
        for item in tree.body
        if isinstance(item, ast.Assign)
        and len(item.targets) == 1
        and isinstance(item.targets[0], ast.Name)
        and item.targets[0].id.startswith("CONF_")
        and isinstance(item.value, ast.Constant)
        and isinstance(item.value.value, str)
    }
    fn = next(item for item in tree.body
              if isinstance(item, ast.FunctionDef)
              and item.name == "validate_fatfs_test_flags")
    module = ast.Module(body=[fn], type_ignores=[])
    ast.fix_missing_locations(module)
    namespace = dict(constants, cv=FakeCV)
    exec(compile(module, str(CONFIG), "exec"), namespace)
    return namespace["validate_fatfs_test_flags"], constants


class FatfsModes(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        validator, cls.constants = load_validator()
        cls.validate = staticmethod(validator)
        cls.modes = [
            val for key, val in cls.constants.items()
            if key.startswith("CONF_FATFS_") and key not in ("CONF_FATFS_MOUNT_TEST",)
        ] + [
            cls.constants[k] for k in
            ("CONF_ASSET_DISCOVERY_PROBE", "CONF_BOUNDED_CRC_PROBE", "CONF_COMBINED_READONLY_PROBE", "CONF_MANIFEST_READONLY_PROBE", "CONF_CACHE_WRITE_PILOT", "CONF_HTTPS_CACHE_PILOT", "CONF_CACHE_READ_PILOT", "CONF_CACHE_SCREEN_PILOT", "CONF_STORAGE_SERVICE_TEST", "CONF_STORAGE_REPEAT_TEST",
             "CONF_SETTINGS_PERSISTENCE_PROBE")
        ]

    def config(self, enabled=(), mount=True):
        result = {key: False for key in self.modes}
        result["fatfs_mount_test"] = mount
        result.update({key: True for key in enabled})
        return result

    def test_each_mode_requires_mount(self):
        for mode in self.modes:
            with self.subTest(mode=mode):
                self.assertEqual(self.validate(self.config([mode])), self.config([mode]))
                with self.assertRaisesRegex(FakeCV.Invalid, "require fatfs_mount_test"):
                    self.validate(self.config([mode], mount=False))

    def test_modes_are_mutually_exclusive(self):
        for i, first in enumerate(self.modes):
            for second in self.modes[i+1:]:
                with self.subTest(first=first, second=second):
                    with self.assertRaisesRegex(FakeCV.Invalid, "Only one FatFs test mode"):
                        self.validate(self.config([first, second]))

    def test_mount_only_is_valid(self):
        self.validate(self.config())

    def test_smoke_fixture_matches_hardware_batch_mode(self):
        fixture = FIXTURE.read_text(encoding="utf-8")
        block = fixture.split("\natilla_storage:\n", 1)[1]
        parsed = dict(re.findall(r"^  (\w+): (true|false)$", block, re.M))
        config = {k: v == "true" for k, v in parsed.items()}
        for mode in self.modes:
            config.setdefault(mode, False)
        self.validate(config)
        self.assertTrue(config["fatfs_mount_test"])
        self.assertTrue(config["fatfs_batch_read_probe"])
        self.assertEqual(sum(config[m] for m in self.modes), 1)
        self.assertFalse(config["fatfs_write_roundtrip_test"])

    def test_batch_worker_is_dispatched_off_main_loop(self):
        header = HEADER.read_text(encoding="utf-8")
        dispatch = header.split("void loop() override", 1)[1].split("static void storage_worker_", 1)[0]
        self.assertIn("fatfs_batch_read_probe_", dispatch)
        self.assertIn("manifest_readonly_probe_", dispatch)
        self.assertIn("cache_write_pilot_", dispatch)
        self.assertIn("https_cache_pilot_", dispatch)
        self.assertIn("cache_read_pilot_", dispatch)
        self.assertIn("cache_screen_pilot_", dispatch)
        self.assertIn("xTaskCreatePinnedToCore", dispatch)
        self.assertRegex(dispatch, r"xTaskCreatePinnedToCore\([\s\S]*?24576")


    def test_cache_handoff_is_blocked_when_unmount_fails(self):
        header = HEADER.read_text(encoding="utf-8")
        fatfs = (ROOT / "components/atilla_storage/fatfs_readonly.h").read_text(
            encoding="utf-8")
        self.assertIn("const bool unmount_ok = fatfs.unmount();", header)
        self.assertIn("cache_image::handoff_ready(cache_read_pilot_ || cache_screen_pilot_, unmount_ok,", header)
        self.assertIn("bool unmount()", fatfs)
        self.assertIn("return unmount_rc == FR_OK;", fatfs)


    def test_preview_mode_is_bounded_and_read_only(self):
        header = HEADER.read_text(encoding="utf-8")
        self.assertIn("const unsigned cache_slot = cache_screen_pilot_ ? 2U : 1U;", header)
        self.assertIn("load_cache_png_readonly(cache_slot, 70, 0xF60D2E27U", header)
        self.assertIn("verified.width == 1 && verified.height == 1", header)
        self.assertIn("CACHE SCREEN MAIN-LOOP", header)
        self.assertIn("bool(verified_cache_png.data)", header)
        self.assertIn("cache_preview_callback_(cache_preview_context_, ready->data.get()", header)


if __name__ == "__main__":
    unittest.main()
