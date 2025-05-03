#include "veml6075_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace veml6075_sensor {

static const char *const TAG = "veml6075";

#define REG_CONF 0x00
#define REG_UVA 0x07
#define REG_UVB 0x09
#define REG_UVCOMP1 0x0A
#define REG_UVCOMP2 0x0B

void VEML6075Sensor::setup() {
  // Apply config once at startup
  uint16_t config = (integration_time_ << 4);  // bits 6:4
  
  if (high_dynamic_) {
    config |= (1 << 3);  // HD
  }
  
  if (mode_ == MODE_FORCED) {
    config |= (1 << 2);  // UV_AF
    config |= (1 << 1);  // TRIG
    config |= (1 << 0);  // SD = 1 (shutdown required for forced mode)
  } else {
    if (shutdown_) {
      config |= (1 << 0);  // SD
    }
  }
  
  this->write_u16_(REG_CONF, config);

  delay(100);

  uint16_t readback = this->read_u16_(REG_CONF);
  ESP_LOGI(TAG, "Setup complete, CONF reg = 0x%04X", readback);
}

void VEML6075Sensor::update() {
  // Reapply config in case sensor resets itself
  uint16_t config = (integration_time_ << 4);  // bits 6:4
  
  if (high_dynamic_) {
    config |= (1 << 3);  // HD
  }
  
  if (mode_ == MODE_FORCED) {
    config |= (1 << 2);  // UV_AF
    config |= (1 << 1);  // TRIG
    config |= (1 << 0);  // SD = 1 (shutdown required for forced mode)
  } else {
    if (shutdown_) {
      config |= (1 << 0);  // SD
    }
  }
  
  this->write_u16_(REG_CONF, config);

  delay(20);

  uint16_t conf = this->read_u16_(REG_CONF);
  ESP_LOGD(TAG, "CONF reg during update: 0x%04X", conf);

  delay(220);  // Allow full integration

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
