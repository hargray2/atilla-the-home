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
CONF_FATFS_FILE_READ_TEST = "fatfs_file_read_test"
CONF_FATFS_ENTRY_PROBE = "fatfs_entry_probe"
CONF_FATFS_FIL_HEAP_PROBE = "fatfs_fil_heap_probe"
CONF_FATFS_OPEN_ONLY_PROBE = "fatfs_open_only_probe"
CONF_FATFS_BATCH_READ_PROBE = "fatfs_batch_read_probe"
CONF_ASSET_DISCOVERY_PROBE = "asset_discovery_probe"
CONF_BOUNDED_CRC_PROBE = "bounded_crc_probe"
CONF_COMBINED_READONLY_PROBE = "combined_readonly_probe"
CONF_MANIFEST_READONLY_PROBE = "manifest_readonly_probe"
CONF_CACHE_WRITE_PILOT = "cache_write_pilot"
CONF_HTTPS_CACHE_PILOT = "https_cache_pilot"
CONF_CACHE_READ_PILOT = "cache_read_pilot"
CONF_FATFS_WRITE_ROUNDTRIP_TEST = "fatfs_write_roundtrip_test"
CONF_STORAGE_SERVICE_TEST = "storage_service_test"
CONF_STORAGE_REPEAT_TEST = "storage_repeat_test"
CONF_SETTINGS_PERSISTENCE_PROBE = "settings_persistence_probe"

def validate_fatfs_test_flags(config):
    active = sum(bool(config[key]) for key in (
        CONF_FATFS_ENTRY_PROBE, CONF_FATFS_FIL_HEAP_PROBE,
        CONF_FATFS_OPEN_ONLY_PROBE, CONF_FATFS_BATCH_READ_PROBE, CONF_ASSET_DISCOVERY_PROBE, CONF_BOUNDED_CRC_PROBE, CONF_COMBINED_READONLY_PROBE, CONF_MANIFEST_READONLY_PROBE, CONF_CACHE_WRITE_PILOT, CONF_HTTPS_CACHE_PILOT, CONF_CACHE_READ_PILOT,
        CONF_FATFS_WRITE_ROUNDTRIP_TEST, CONF_STORAGE_SERVICE_TEST, CONF_STORAGE_REPEAT_TEST, CONF_SETTINGS_PERSISTENCE_PROBE,
        CONF_FATFS_FILE_READ_TEST))
    if active > 1:
        raise cv.Invalid("Only one FatFs test mode may be enabled at a time")
    if active and not config[CONF_FATFS_MOUNT_TEST]:
        raise cv.Invalid("FatFs test modes require fatfs_mount_test: true")
    return config

CONFIG_SCHEMA = cv.All(cv.Schema({
    cv.GenerateID(): cv.declare_id(AtillaStorage),
    cv.Required(CONF_CARD_DEVICE): cv.use_id(spi_device.spi_device),
    cv.Required(CONF_IDLE_DEVICE): cv.use_id(spi_device.spi_device),
    cv.Optional(CONF_FATFS_MOUNT_TEST, default=False): cv.boolean,
    cv.Optional(CONF_FATFS_FILE_READ_TEST, default=False): cv.boolean,
    cv.Optional(CONF_FATFS_ENTRY_PROBE, default=False): cv.boolean,
    cv.Optional(CONF_FATFS_FIL_HEAP_PROBE, default=False): cv.boolean,
    cv.Optional(CONF_FATFS_OPEN_ONLY_PROBE, default=False): cv.boolean,
    cv.Optional(CONF_FATFS_BATCH_READ_PROBE, default=False): cv.boolean,
    cv.Optional(CONF_ASSET_DISCOVERY_PROBE, default=False): cv.boolean,
    cv.Optional(CONF_BOUNDED_CRC_PROBE, default=False): cv.boolean,
    cv.Optional(CONF_COMBINED_READONLY_PROBE, default=False): cv.boolean,
    cv.Optional(CONF_MANIFEST_READONLY_PROBE, default=False): cv.boolean,
    cv.Optional(CONF_CACHE_WRITE_PILOT, default=False): cv.boolean,
    cv.Optional(CONF_HTTPS_CACHE_PILOT, default=False): cv.boolean,
    cv.Optional(CONF_CACHE_READ_PILOT, default=False): cv.boolean,
    cv.Optional(CONF_FATFS_WRITE_ROUNDTRIP_TEST, default=False): cv.boolean,
    cv.Optional(CONF_STORAGE_SERVICE_TEST, default=False): cv.boolean,
    cv.Optional(CONF_STORAGE_REPEAT_TEST, default=False): cv.boolean,
    cv.Optional(CONF_SETTINGS_PERSISTENCE_PROBE, default=False): cv.boolean,
}).extend(cv.COMPONENT_SCHEMA), validate_fatfs_test_flags)

async def to_code(config):
    # Native ESP-IDF builds exclude FatFs by default unless requested.
    esp32.include_builtin_idf_component("fatfs")
    esp32.include_builtin_idf_component("esp_http_client")
    esp32.include_builtin_idf_component("esp-tls")
    esp32.include_builtin_idf_component("mbedtls")
    # TLS validation must have an embedded trusted root certificate bundle.
    esp32.add_idf_sdkconfig_option("CONFIG_MBEDTLS_CERTIFICATE_BUNDLE", True)
    esp32.require_fatfs()
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    card = await cg.get_variable(config[CONF_CARD_DEVICE])
    idle = await cg.get_variable(config[CONF_IDLE_DEVICE])
    cg.add(var.set_devices(card, idle))
    cg.add(var.set_fatfs_mount_test(config[CONF_FATFS_MOUNT_TEST]))
    cg.add(var.set_fatfs_file_read_test(config[CONF_FATFS_FILE_READ_TEST]))
    cg.add(var.set_fatfs_entry_probe(config[CONF_FATFS_ENTRY_PROBE]))
    cg.add(var.set_fatfs_fil_heap_probe(config[CONF_FATFS_FIL_HEAP_PROBE]))
    cg.add(var.set_fatfs_open_only_probe(config[CONF_FATFS_OPEN_ONLY_PROBE]))
    cg.add(var.set_fatfs_batch_read_probe(config[CONF_FATFS_BATCH_READ_PROBE]))
    cg.add(var.set_asset_discovery_probe(config[CONF_ASSET_DISCOVERY_PROBE]))
    cg.add(var.set_bounded_crc_probe(config[CONF_BOUNDED_CRC_PROBE]))
    cg.add(var.set_combined_readonly_probe(config[CONF_COMBINED_READONLY_PROBE]))
    cg.add(var.set_manifest_readonly_probe(config[CONF_MANIFEST_READONLY_PROBE]))
    cg.add(var.set_cache_write_pilot(config[CONF_CACHE_WRITE_PILOT]))
    cg.add(var.set_https_cache_pilot(config[CONF_HTTPS_CACHE_PILOT]))
    cg.add(var.set_cache_read_pilot(config[CONF_CACHE_READ_PILOT]))
    cg.add(var.set_fatfs_write_roundtrip_test(config[CONF_FATFS_WRITE_ROUNDTRIP_TEST]))
    cg.add(var.set_storage_service_test(config[CONF_STORAGE_SERVICE_TEST]))
    cg.add(var.set_storage_repeat_test(config[CONF_STORAGE_REPEAT_TEST]))
    cg.add(var.set_settings_persistence_probe(config[CONF_SETTINGS_PERSISTENCE_PROBE]))
