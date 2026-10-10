"""ATILLA 4.6 experimental read-only ESPHome SD component.

This integration uses the two existing generic ESPHome spi_device entities.
No filesystem mount or writes are enabled.
"""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import spi_device, esp32

DEPENDENCIES = ["spi_device"]

atilla_ns = cg.esphome_ns.namespace("atilla_storage")
AtillaStorage = atilla_ns.class_("AtillaStorage", cg.Component)

CONF_CARD_DEVICE = "card_device"
CONF_IDLE_DEVICE = "idle_device"
CONF_FATFS_MOUNT_TEST = "fatfs_mount_test"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(AtillaStorage),
    cv.Required(CONF_CARD_DEVICE): cv.use_id(spi_device.spi_device),
    cv.Required(CONF_IDLE_DEVICE): cv.use_id(spi_device.spi_device),
    cv.Optional(CONF_FATFS_MOUNT_TEST, default=False): cv.boolean,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    # Native ESP-IDF builds exclude FatFs by default unless requested.
    esp32.include_builtin_idf_component("fatfs")
    esp32.require_fatfs()
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    card = await cg.get_variable(config[CONF_CARD_DEVICE])
    idle = await cg.get_variable(config[CONF_IDLE_DEVICE])
    cg.add(var.set_devices(card, idle))
    cg.add(var.set_fatfs_mount_test(config[CONF_FATFS_MOUNT_TEST]))
