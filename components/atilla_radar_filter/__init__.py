"""ATILLA radar-only precipitation palette filter. Experimental / opt-in."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components.online_image.image import OnlineImage
from esphome.const import CONF_ID

# `online_image` is now an `image:` platform in ESPHome 2026.9.1.
# A DEPENDENCIES entry requires a top-level `online_image:` block and fails
# when only modern `image: - platform: online_image` entries are configured.
# The typed cv.use_id(OnlineImage) validator still requires a real image ID.
AUTO_LOAD = ["image", "runtime_image"]

ns = cg.esphome_ns.namespace("atilla_radar_filter")
RadarFilter = ns.class_("RadarFilter", cg.Component)
FilterAction = ns.class_("FilterAction", automation.Action)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(RadarFilter),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    obj = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(obj, config)

@automation.register_action(
    "atilla_radar_filter.apply",
    FilterAction,
    cv.Schema({
        cv.GenerateID(): cv.use_id(RadarFilter),
        cv.Required("image_id"): cv.use_id(OnlineImage),
    }),
    synchronous=True,
)
async def apply_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    img = await cg.get_variable(config["image_id"])
    return cg.new_Pvariable(action_id, template_arg, parent, img)
