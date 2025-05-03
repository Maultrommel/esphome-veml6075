import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_ULTRAVIOLET_INDEX,
    STATE_CLASS_MEASUREMENT,
    UNIT_UV_INDEX,
)

DEPENDENCIES = ["i2c"]

veml6075_ns = cg.esphome_ns.namespace("veml6075_sensor")
VEML6075Component = veml6075_ns.class_("VEML6075Component", cg.PollingComponent, i2c.I2CDevice)

CONFIG_SCHEMA = (
    cv.Schema({
        cv.GenerateID(): cv.declare_id(VEML6075Component),
        cv.Optional("integration_time", default="100ms"): cv.one_of("50ms", "100ms", "200ms", "400ms", "800ms", lower=True),
        cv.Optional("mode", default="active"): cv.one_of("active", "force", lower=True),
        cv.Optional("uv_index"): sensor.sensor_schema(
            unit_of_measurement=UNIT_UV_INDEX,
            device_class=DEVICE_CLASS_ULTRAVIOLET_INDEX,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional("uva"): sensor.sensor_schema(),
        cv.Optional("uvb"): sensor.sensor_schema(),
        cv.Optional("uvcomp1"): sensor.sensor_schema(),
        cv.Optional("uvcomp2"): sensor.sensor_schema(),
    }).extend(cv.polling_component_schema("60s")).extend(i2c.i2c_device_schema())
)

async def to_code(config):
    var = await i2c.new_i2c_device(config)
    comp = cg.new_Pvariable(config[CONF_ID], var)

    await cg.register_component(comp, config)
    await i2c.register_i2c_device(comp, config)

    comp.set_integration_time(config["integration_time"])
    comp.set_mode(config["mode"])

    if "uv_index" in config:
        sens = await sensor.new_sensor(config["uv_index"])
        cg.add(comp.set_uv_index_sensor(sens))

    if "uva" in config:
        sens = await sensor.new_sensor(config["uva"])
        cg.add(comp.set_uva_sensor(sens))

    if "uvb" in config:
        sens = await sensor.new_sensor(config["uvb"])
        cg.add(comp.set_uvb_sensor(sens))

    if "uvcomp1" in config:
        sens = await sensor.new_sensor(config["uvcomp1"])
        cg.add(comp.set_uvcomp1_sensor(sens))

    if "uvcomp2" in config:
        sens = await sensor.new_sensor(config["uvcomp2"])
        cg.add(comp.set_uvcomp2_sensor(sens))
