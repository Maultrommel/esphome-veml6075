from esphome.codegen import codegen
from esphome.components import i2c, sensor
from esphome.const import CONF_ID

from . import veml6075_ns

CONF_INTEGRATION_TIME = "integration_time"
CONF_MODE = "mode"

VEML6075Sensor = veml6075_ns.class_("VEML6075Sensor", sensor.Sensor, i2c.I2CDevice)

def to_code(config):
    var = yield i2c.new_i2c_device(config)
    sens = yield sensor.new_sensor(config[CONF_ID])
    cg = codegen.get_variable

    veml = cg.new_Pvariable(config[CONF_ID], sens)
    veml.set_integration_time(config[CONF_INTEGRATION_TIME])
    veml.set_mode(config[CONF_MODE])
    yield i2c.register_i2c_device(veml, config)

    if "uv_index" in config:
        sens = yield sensor.new_sensor(config["uv_index"])
        veml.set_uv_index_sensor(sens)
    if "uva" in config:
        sens = yield sensor.new_sensor(config["uva"])
        veml.set_uva_sensor(sens)
    if "uvb" in config:
        sens = yield sensor.new_sensor(config["uvb"])
        veml.set_uvb_sensor(sens)
    if "uvcomp1" in config:
        sens = yield sensor.new_sensor(config["uvcomp1"])
        veml.set_uvcomp1_sensor(sens)
    if "uvcomp2" in config:
        sens = yield sensor.new_sensor(config["uvcomp2"])
        veml.set_uvcomp2_sensor(sens)
