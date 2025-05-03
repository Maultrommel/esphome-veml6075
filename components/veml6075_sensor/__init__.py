import esphome.config_validation as cv
from esphome import automation
from esphome.const import (
    CONF_ID,
    STATE_CLASS_MEASUREMENT,
    UNIT_UV_INDEX,
)
from esphome.components import i2c, sensor
from esphome.components.sensor import Sensor
import esphome.codegen as cg

CODEOWNERS = ["@Maultrommel"]
DEPENDENCIES = ["i2c"]

veml6075_sensor_ns = cg.esphome_ns.namespace("veml6075_sensor")
VEML6075SensorComponent = veml6075_sensor_ns.class_(
    "VEML6075SensorComponent", cg.Component, i2c.I2CDevice
)

CONF_UV_INDEX = "uv_index"
CONF_UVA = "uva"
CONF_UVB = "uvb"
CONF_UVCOMP1 = "uvcomp1"
CONF_UVCOMP2 = "uvcomp2"
CONF_INTEGRATION_TIME = "integration_time"
CONF_MODE = "mode"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(VEML6075SensorComponent),
        cv.Optional(CONF_INTEGRATION_TIME, default="100ms"): cv.one_of(
            "50ms", "100ms", "200ms", "400ms", "800ms", lower=True
        ),
        cv.Optional(CONF_MODE, default="normal"): cv.one_of(
            "normal", "active", lower=True
        ),
        cv.Optional(CONF_UV_INDEX): sensor.sensor_schema(
            unit_of_measurement=UNIT_UV_INDEX,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_UVA): sensor.sensor_schema(
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_UVB): sensor.sensor_schema(
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_UVCOMP1): sensor.sensor_schema(
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_UVCOMP2): sensor.sensor_schema(
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x10))

async def to_code(config):
    var = await cg.new_device(config)

    cg.add(var.set_integration_time(config[CONF_INTEGRATION_TIME]))
    cg.add(var.set_mode(config[CONF_MODE]))

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

