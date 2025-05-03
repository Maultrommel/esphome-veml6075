#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace veml6075_sensor {

enum VEML6075IntegrationTime {
  IT_50MS = 0,
  IT_100MS,
  IT_200MS,
  IT_400MS,
  IT_800MS
};

enum VEML6075Mode {
  MODE_ACTIVE,
  MODE_FORCED
};

class VEML6075Sensor : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_integration_time(VEML6075IntegrationTime time) { integration_time_ = time; }
  void set_mode(VEML6075Mode mode) { mode_ = mode; }
  void set_shutdown(bool shutdown) { shutdown_ = shutdown; }
  void set_high_dynamic(bool high_dynamic) { high_dynamic_ = high_dynamic; }

  void set_uv_index_sensor(sensor::Sensor *s) { uv_index_sensor_ = s; }
  void set_uva_sensor(sensor::Sensor *s) { uva_sensor_ = s; }
  void set_uvb_sensor(sensor::Sensor *s) { uvb_sensor_ = s; }
  void set_uvcomp1_sensor(sensor::Sensor *s) { uvcomp1_sensor_ = s; }
  void set_uvcomp2_sensor(sensor::Sensor *s) { uvcomp2_sensor_ = s; }

  void setup() override;
  void update() override;

 protected:
  void configure_sensor_();
  uint16_t read_u16_(uint8_t reg);
  void write_u16_(uint8_t reg, uint16_t value);
  float calculate_uvi_(float comp_uva, float comp_uvb);
  float get_comp_uva_(uint16_t uva, uint16_t uvcomp1, uint16_t uvcomp2);
  float get_comp_uvb_(uint16_t uvb, uint16_t uvcomp1, uint16_t uvcomp2);

  VEML6075IntegrationTime integration_time_ = IT_100MS;
  VEML6075Mode mode_ = MODE_ACTIVE;
  bool shutdown_{false};
  bool high_dynamic_{false};

  sensor::Sensor *uv_index_sensor_{nullptr};
  sensor::Sensor *uva_sensor_{nullptr};
  sensor::Sensor *uvb_sensor_{nullptr};
  sensor::Sensor *uvcomp1_sensor_{nullptr};
  sensor::Sensor *uvcomp2_sensor_{nullptr};
};

}  // namespace veml6075_sensor
}  // namespace esphome
