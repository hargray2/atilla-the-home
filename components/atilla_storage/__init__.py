"""ATILLA SD storage — initial ESPHome external component scaffold.

This first version intentionally does not touch the SD card. The C++
FATFS adapter is not yet validated for ESPHome 2026.9.1 and CH422G CS.
"""

import esphome.config_validation as cv

CONFIG_SCHEMA = cv.Schema({})

async def to_code(config):
    # No runtime behaviour until the read-only adapter has been validated.
    pass
