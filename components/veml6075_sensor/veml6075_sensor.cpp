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
  // Set integration time to 100ms, SD = 0 (power on)
  uint16_t config = (0b001 << 4);  // IT[6:4] = 001 => 100ms
  config &= ~(1 << 0);             // SD = 0 => power on
  this->write_u16_(REG_CONF, config);
  delay(100);  // wait for sensor to wake

  for (uint8_t reg = 0x00; reg <= 0x0B; reg++) {
  uint16_t val = this->read_u16_(reg);
  ESP_LOGI(TAG, "Reg 0x%02X: 0x%04X", reg, val);
}

  // Read back config for confirmation
  uint16_t readback = this->read_u16_(REG_CONF);
  ESP_LOGI(TAG, "Config register readback: 0x%04X", readback);

  ESP_LOGI(TAG, "Setup complete");
}

void VEML6075Sensor::update() {
  delay(150);  // integration time
  delay(20);   // ensure previous write settles

  uint16_t uva = read_u16_(REG_UVA);
  uint16_t uvb = read_u16_(REG_UVB);
  uint16_t uvcomp1 = read_u16_(REG_UVCOMP1);
  uint16_t uvcomp2 = read_u16_(REG_UVCOMP2);

  ESP_LOGD(TAG, "Raw UVA: %u", uva);
  ESP_LOGD(TAG, "Raw UVB: %u", uvb);
  ESP_LOGD(TAG, "UVCOMP1: %u", uvcomp1);
  ESP_LOGD(TAG, "UVCOMP2: %u", uvcomp2);

  float comp_uva = get_comp_uva_(uva, uvcomp1, uvcomp2);
  float comp_uvb = get_comp_uvb_(uvb, uvcomp1, uvcomp2);
  ESP_LOGD(TAG, "Compensated UVA: %.2f", comp_uva);
  ESP_LOGD(TAG, "Compensated UVB: %.2f", comp_uvb);

  float uvi = calculate_uvi_(comp_uva, comp_uvb);
  if (uvi < 0.0f)
    uvi = 0.0f;

  if (uv_index_sensor_) uv_index_sensor_->publish_state(uvi);
  if (uva_sensor_) uva_sensor_->publish_state(uva);
  if (uvb_sensor_) uvb_sensor_->publish_state(uvb);
  if (uvcomp1_sensor_) uvcomp1_sensor_->publish_state(uvcomp1);
  if (uvcomp2_sensor_) uvcomp2_sensor_->publish_state(uvcomp2);
}

uint16_t VEML6075Sensor::read_u16_(uint8_t reg) {
  uint8_t buffer[2] = {0x00, 0x00};

  if (!this->read_bytes(reg, buffer, 2)) {
    ESP_LOGE(TAG, "Failed to read register 0x%02X", reg);
    return 0;
  }

  ESP_LOGD(TAG, "Read reg 0x%02X: [%02X %02X]", reg, buffer[0], buffer[1]);
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
