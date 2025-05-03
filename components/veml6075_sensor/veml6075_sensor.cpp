#include "veml6075_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace veml6075_sensor {

static const char *const TAG = "veml6075";

const float HD_SCALAR = 2.0f;
const float UVA_RESP_100MS = 0.001111;  // uncovered, from Vishay/SparkFun
const float UVB_RESP_100MS = 0.00125;

const float UVA_RESPONSIVITY[] = {
  UVA_RESP_100MS / 0.5016286645f,  // 50ms
  UVA_RESP_100MS,                 // 100ms
  UVA_RESP_100MS / 2.039087948f,  // 200ms
  UVA_RESP_100MS / 3.781758958f,  // 400ms
  UVA_RESP_100MS / 7.371335505f   // 800ms
};

const float UVB_RESPONSIVITY[] = {
  UVB_RESP_100MS / 0.5016286645f,
  UVB_RESP_100MS,
  UVB_RESP_100MS / 2.039087948f,
  UVB_RESP_100MS / 3.781758958f,
  UVB_RESP_100MS / 7.371335505f
};

#define REG_CONF    0x00
#define REG_UVA     0x07
#define REG_UVB     0x09
#define REG_UVCOMP1 0x0A
#define REG_UVCOMP2 0x0B


// REG_CONF bit masks and shifts (from SparkFun library)
#define VEML6075_UV_IT_MASK     0x70  // bits 6:4
#define VEML6075_UV_IT_SHIFT    4
#define VEML6075_HD_MASK        0x08  // bit 3
#define VEML6075_TRIG_MASK      0x04  // bit 2
#define VEML6075_AF_MASK        0x02  // bit 1
#define VEML6075_SHUTDOWN_MASK  0x01  // bit 0


void VEML6075Sensor::setup() {
  // Start with a clean config
  uint16_t config = 0x00;

  // Integration time (bits 6:4)
  config |= ((integration_time_ << VEML6075_UV_IT_SHIFT) & VEML6075_UV_IT_MASK);

  if (high_dynamic_)
    config |= VEML6075_HD_MASK;

  if (mode_ == MODE_FORCED) {
    config |= VEML6075_AF_MASK;       // Set forced mode
    config |= VEML6075_TRIG_MASK;     // Trigger measurement
    config |= VEML6075_SHUTDOWN_MASK; // Must be in shutdown for forced mode
  } else {
    if (shutdown_)
      config |= VEML6075_SHUTDOWN_MASK;
  }

  this->write_u16_(REG_CONF, config);
  delay(100);

  uint16_t readback = this->read_u16_(REG_CONF);
  ESP_LOGI(TAG, "Setup complete, CONF reg = 0x%04X", readback);
}

void VEML6075Sensor::update() {
  // Avoid re-reading too fast
  const unsigned long now = millis();
  if ((now - last_read_time_) < 1000) {
    ESP_LOGD(TAG, "Skipping update (read too soon)");
    return;
  }
  last_read_time_ = now;

  // Trigger a forced measurement (if in forced mode)
  uint16_t config = (integration_time_ << VEML6075_UV_IT_SHIFT) & VEML6075_UV_IT_MASK;
  if (high_dynamic_) config |= VEML6075_HD_MASK;

  if (mode_ == MODE_FORCED) {
    config |= VEML6075_AF_MASK | VEML6075_TRIG_MASK | VEML6075_SHUTDOWN_MASK;
    this->write_u16_(REG_CONF, config);
    delay(150);  // Wait for integration
  }

  // Read all raw values
  const uint16_t uva = read_u16_(REG_UVA);
  const uint16_t uvb = read_u16_(REG_UVB);
  const uint16_t uvcomp1 = read_u16_(REG_UVCOMP1);
  const uint16_t uvcomp2 = read_u16_(REG_UVCOMP2);

  // Compensate UVA and UVB
  float comp_uva = static_cast<float>(uva) - (uva_a_coef * uvcomp1) - (uva_b_coef * uvcomp2);
  float comp_uvb = static_cast<float>(uvb) - (uvb_c_coef * uvcomp1) - (uvb_d_coef * uvcomp2);

  if (high_dynamic_) {
    comp_uva /= HD_SCALAR;
    comp_uvb /= HD_SCALAR;
  }

  // Use SparkFun's index method
  float uvi = (comp_uva * UVA_RESPONSIVITY[integration_time_] +
               comp_uvb * UVB_RESPONSIVITY[integration_time_]) / 2.0f;

  // Clamp values
  last_uva_ = std::max(0.0f, comp_uva);
  last_uvb_ = std::max(0.0f, comp_uvb);
  last_index_ = std::max(0.0f, uvi);

  // Publish
  if (uva_sensor_) uva_sensor_->publish_state(uva);
  if (uvb_sensor_) uvb_sensor_->publish_state(uvb);
  if (uvcomp1_sensor_) uvcomp1_sensor_->publish_state(uvcomp1);
  if (uvcomp2_sensor_) uvcomp2_sensor_->publish_state(uvcomp2);
  if (uv_index_sensor_) uv_index_sensor_->publish_state(last_index_);

  ESP_LOGD(TAG, "UVA: %.2f  UVB: %.2f  UVI: %.3f", last_uva_, last_uvb_, last_index_);
}

uint16_t VEML6075Sensor::read_u16_(uint8_t reg) {
  uint8_t buffer[2] = {0x00, 0x00};
  if (!this->read_bytes(reg, buffer, 2)) {
    ESP_LOGE(TAG, "Failed to read reg 0x%02X", reg);
    return 0;
  }
  return (uint16_t(buffer[1]) << 8) | buffer[0];
}

void VEML6075Sensor::write_u16_(uint8_t reg, uint16_t value) {
  uint8_t buffer[2];
  buffer[0] = value & 0xFF;        // LSB
  buffer[1] = (value >> 8) & 0xFF; // MSB
  this->write_bytes(reg, buffer, 2);
}

float VEML6075Sensor::get_comp_uva_(uint16_t uva, uint16_t uvcomp1, uint16_t uvcomp2) {
  return float(uva) - (2.22f * uvcomp1) - (1.33f * uvcomp2);
}

float VEML6075Sensor::get_comp_uvb_(uint16_t uvb, uint16_t uvcomp1, uint16_t uvcomp2) {
  return float(uvb) - (2.95f * uvcomp1) - (1.74f * uvcomp2);
}

float VEML6075Sensor::calculate_uvi_(float comp_uva, float comp_uvb) {
  return (comp_uva + comp_uvb) / 2.0f * 0.0011f;
}

}  // namespace veml6075_sensor
}  // namespace esphome
