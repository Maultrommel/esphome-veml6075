import esphome.codegen as cg
import esphome.config_validation as cv
import voluptuous as vol
from esphome.components import sensor

# Define the namespace for the component
veml6075_ns = cg.esphome_ns.namespace("veml6075_sensor")

# Define a simplified schema that matches the YAML structure
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

# Function to initialize the sensor based on the provided configuration
def setup(veml6075_sensor, config):
    veml6075_sensor.integration_time = config["integration_time"]
    veml6075_sensor.mode = config["mode"]

    # Initialize the sensor values (uv_index, uva, etc.)
    if "uv_index" in config:
        veml6075_sensor.uv_index = config["uv_index"]
    if "uva" in config:
        veml6075_sensor.uva = config["uva"]
    if "uvb" in config:
        veml6075_sensor.uvb = config["uvb"]
    if "uvcomp1" in config:
        veml6075_sensor.uvcomp1 = config["uvcomp1"]
    if "uvcomp2" in config:
        veml6075_sensor.uvcomp2 = config["uvcomp2"]
