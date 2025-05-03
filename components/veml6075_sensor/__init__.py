import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    STATE_CLASS_MEASUREMENT,
    UNIT_EMPTY,
)

# Namespace and class name must match C++ code
veml6075_sensor_ns = cg.esphome_ns.namespace("veml6075_sensor")
VEML6075Sensor = veml6075_sensor_ns.class_(
    "VEML6075Sensor", cg.PollingComponent, i2c.I2CDevice
)

# Configuration keys
CONF_UV_INDEX = "uv_index"
CONF_UVA = "uva"
CONF_UVB = "uvb"
CONF_UVCOMP1 = "uvcomp1"
CONF_UVCOMP2 = "uvcomp2"
CONF_INTEGRATION_TIME = "integration_time"
CONF_MODE = "mode"

INTEGRATION_TIMES = {
    "50ms": veml6075_sensor_ns.enum("IT_50MS"),
    "100ms": veml6075_sensor_ns.enum("IT_100MS"),
    "200ms": veml6075_sensor_ns.enum("IT_200MS"),
    "400ms": veml6075_sensor_ns.enum("IT_400MS"),
    "800ms": veml6075_sensor_ns.enum("IT_800MS"),
}

MODES = {
    "active": veml6075_sensor_ns.enum("MODE_ACTIVE"),
    "forced": veml6075_sensor_ns.enum("MODE_FORCED"),
}

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(VEML6075Sensor),
    cv.Optional(CONF_INTEGRATION_TIME, default="100ms"): cv.enum(INTEGRATION_TIMES, upper=False),
    cv.Optional(CONF_MODE, default="active"): cv.enum(MODES, upper=False),
    cv.Optional("shutdown", default=False): cv.boolean,
    cv.Optional("force_mode", default=False): cv.boolean,
    cv.Optional("high_dynamic", default=False): cv.boolean,
    cv.Optional(CONF_UV_INDEX): sensor.sensor_schema(
        unit_of_measurement="UV Index",
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_UVA): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_UVB): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_UVCOMP1): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_UVCOMP2): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=2,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
}).extend(cv.polling_component_schema("60s")).extend(i2c.i2c_device_schema(0x10))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_integration_time(config[CONF_INTEGRATION_TIME]))
    cg.add(var.set_mode(config[CONF_MODE]))
    cg.add(var.set_shutdown(config["shutdown"]))
    cg.add(var.set_force_mode(config["force_mode"]))
    cg.add(var.set_high_dynamic(config["high_dynamic"]))

    if CONF_UV_INDEX in config:
        sens = await sensor.new_sensor(config[CONF_UV_INDEX])
        cg.add(var.set_uv_index_sensor(sens))

    if CONF_UVA in config:
        sens = await sensor.new_sensor(config[CONF_UVA])
        cg.add(var.set_uva_sensor(sens))

    if CONF_UVB in config:
        sens = await sensor.new_sensor(config[CONF_UVB])
        cg.add(var.set_uvb_sensor(sens))

    if CONF_UVCOMP1 in config:
        sens = await sensor.new_sensor(config[CONF_UVCOMP1])
        cg.add(var.set_uvcomp1_sensor(sens))

    if CONF_UVCOMP2 in config:
        sens = await sensor.new_sensor(config[CONF_UVCOMP2])
        cg.add(var.set_uvcomp2_sensor(sens))
