import esphome.codegen as cg
import esphome.config_validation as cv
import voluptuous as vol
from esphome.components import sensor

# Define the namespace for the component
veml6075_ns = cg.esphome_ns.namespace("veml6075_sensor")

# Define a simpler schema
CONFIG_SCHEMA = vol.Schema({
    cv.GenerateID(): cv.declare_id(veml6075_ns.Veml6075Sensor),
    vol.Optional("integration_time", default="200ms"): vol.In(["200ms", "400ms", "800ms"]),
    vol.Optional("mode", default="active"): vol.In(["active", "forced"]),
    vol.Optional("uv_index"): sensor.sensor_schema,
    vol.Optional("uva"): sensor.sensor_schema,
    vol.Optional("uvb"): sensor.sensor_schema,
    vol.Optional("uvcomp1"): sensor.sensor_schema,
    vol.Optional("uvcomp2"): sensor.sensor_schema,
}, extra=vol.ALLOW_EXTRA)

# Component setup
def setup(veml6075_sensor, config):
    # Assign the configuration to the sensor
    veml6075_sensor.integration_time = config["integration_time"]
    veml6075_sensor.mode = config["mode"]
    veml6075_sensor.uv_index = config["uv_index"]
    veml6075_sensor.uva = config["uva"]
    veml6075_sensor.uvb = config["uvb"]
    veml6075_sensor.uvcomp1 = config["uvcomp1"]
    veml6075_sensor.uvcomp2 = config["uvcomp2"]
