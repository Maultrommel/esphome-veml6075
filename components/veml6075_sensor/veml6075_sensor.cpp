#include "veml6075_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace veml6075_sensor {

static const char *const TAG = "veml6075";

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
  // Always reapply config to keep sensor alive and clean
  uint16_t config = 0x00;

  config |= ((integration_time_ << VEML6075_UV_IT_SHIFT) & VEML6075_UV_IT_MASK);

  if (high_dynamic_)
    config |= VEML6075_HD_MASK;

  if (mode_ == MODE_FORCED) {
    config |= VEML6075_AF_MASK;
    config |= VEML6075_TRIG_MASK;
    config |= VEML6075_SHUTDOWN_MASK;
  } else {
    if (shutdown_)
      config |= VEML6075_SHUTDOWN_MASK;
  }

  this->write_u16_(REG_CONF, config);
  delay(20);

  uint16_t conf = this->read_u16_(REG_CONF);
  ESP_LOGD(TAG, "CONF reg during update: 0x%04X", conf);

  delay(220);  // Integration delay

  // Read all UV data
  uint16_t uva = read_u16_(REG_UVA);
  uint16_t uvb = read_u16_(REG_UVB);
  uint16_t uvcomp1 = read_u16_(REG_UVCOMP1);
  uint16_t uvcomp2 = read_u16_(REG_UVCOMP2);

  ESP_LOGD(TAG, "UVA: %u  UVB: %u  C1: %u  C2: %u", uva, uvb, uvcomp1, uvcomp2);

  float comp_uva = get_comp_uva_(uva, uvcomp1, uvcomp2);
  float comp_uvb = get_comp_uvb_(uvb, uvcomp1, uvcomp2);

  comp_uva = std::max(0.0f, comp_uva);
  comp_uvb = std::max(0.0f, comp_uvb);

  float uvi = calculate_uvi_(comp_uva, comp_uvb);
  if (uvi < 0.0f) uvi = 0.0f;

  if (uv_index_sensor_) uv_index_sensor_->publish_state(uvi);
  if (uva_sensor_) uva_sensor_->publish_state(uva);
  if (uvb_sensor_) uvb_sensor_->publish_state(uvb);
  if (uvcomp1_sensor_) uvcomp1_sensor_->publish_state(uvcomp1);
  if (uvcomp2_sensor_) uvcomp2_sensor_->publish_state(uvcomp2);
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
