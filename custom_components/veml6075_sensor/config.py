import esphome.config_validation as cv
from esphome import pins
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID, CONF_NAME, CONF_UNIT_OF_MEASUREMENT, CONF_ICON,
    DEVICE_CLASS_ULTRAVIOLET_INDEX, STATE_CLASS_MEASUREMENT,
)

from . import veml6075_ns

VEML6075Sensor = veml6075_ns.class_("VEML6075Sensor", sensor.Sensor, i2c.I2CDevice)

CONF_VEML6075_ID = "veml6075_sensor"
CONF_INTEGRATION_TIME = "integration_time"
CONF_MODE = "mode"

VEML6075_MODES = {
    "active": veml6075_ns.enum("MODE_ACTIVE"),
    "forced": veml6075_ns.enum("MODE_FORCED"),
}

VEML6075_TIMES = {
    "50ms": veml6075_ns.enum("IT_50MS"),
    "100ms": veml6075_ns.enum("IT_100MS"),
    "200ms": veml6075_ns.enum("IT_200MS"),
    "400ms": veml6075_ns.enum("IT_400MS"),
    "800ms": veml6075_ns.enum("IT_800MS"),
}

def sensor_schema(unit, icon=None):
    return sensor.sensor_schema(
        unit_of_measurement=unit,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_ULTRAVIOLET_INDEX if unit == "UVI" else None,
        state_class=STATE_CLASS_MEASUREMENT,
        icon=icon,
    )

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_VEML6075_ID): cv.declare_id(VEML6075Sensor),
    cv.Optional(CONF_INTEGRATION_TIME, default="100ms"): cv.enum(VEML6075_TIMES, upper=False),
    cv.Optional(CONF_MODE, default="active"): cv.enum(VEML6075_MODES, upper=False),
    cv.Optional("uv_index"): sensor_schema("UVI", "mdi:weather-sunny-alert"),
    cv.Optional("uva"): sensor_schema("raw"),
    cv.Optional("uvb"): sensor_schema("raw"),
    cv.Optional("uvcomp1"): sensor_schema("raw"),
    cv.Optional("uvcomp2"): sensor_schema("raw"),
}).extend(i2c.i2c_device_schema(0x10))
