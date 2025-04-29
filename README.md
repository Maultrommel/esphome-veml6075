# ESPHome VEML6075 Component

Custom ESPHome component for the **VEML6075 UVA/UVB sensor** with support for:

- YAML schema validation
- Integration time and active/forced modes
- UV index, UVA, UVB, compensation channels

## Installation

1. Clone or download this repo.
2. Add to your ESPHome project:

```yaml
external_components:
  - source: github://yourusername/esphome-veml6075
    components: [veml6075_sensor]
