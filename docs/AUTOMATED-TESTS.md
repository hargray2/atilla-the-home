# Automated checks and their limits

`tests/fatfs-smoke.yaml` is a deliberately **non-flashable** generic ESP32-S3 CI fixture, not the private Waveshare production firmware. GPIO10/14 chip selects are mock mappings, **not** the actual CH422G EXIO4 SD chip-select hardware path.

## Run locally

```bash
python -m unittest discover -s tests -p 'test_*.py' -v
python -m pip install esphome==2026.9.1
esphome config tests/fatfs-smoke.yaml
esphome compile tests/fatfs-smoke.yaml
```

Unit tests extract the actual `validate_fatfs_test_flags` function from the ESPHome component's Python AST and assert all modes require the mount prerequisite, pairwise mode exclusion, the read-only batch fixture, and off-main-loop FreeRTOS dispatch with a 24 KB task stack. These are **guardrails** only, not a proof that the card can be read. CI compiles the adapter with ESPHome 2026.9.1 and never accesses real hardware.

A GitHub Action runs the tests and fixture validation/compile when relevant component or test paths change on a pull request or configured branch. Do not consider CI green equal to production sign-off. The full single-file firmware still needs private Validate, Compile, OTA and live device evidence.

The previous settings-persistence-only smoke fixture was replaced by a batch-read smoke fixture matching the 4.7.9t verified mode.
