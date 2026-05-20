# Filament Extruder

**DIY PET Bottle Recycling → 3D Printing Filament**

An Arduino-based filament extruder controller that recycles PET plastic bottles into 3D printer filament. Uses a PID temperature controller, I2C LCD display, relay-driven heater, and NTC thermistor for precise temperature regulation at 245°C — the optimal extrusion temperature for PET.

> 📚 **Full documentation:** See [`docs/`](./docs/) for API reference, hardware setup, workflow, and design notes.

## Quick Start

1. Open `FilamentCreator/FilamentCreator.ino` in Arduino IDE
2. Install required libraries: `LiquidCrystal_I2C`, `PID_v1` (Brett Beauregard)
3. Connect hardware per the pinout in [`docs/hardware-setup.md`](./docs/hardware-setup.md)
4. Upload to Arduino Uno/Nano

## Project Structure

```
FilamentCreator/
├── FilamentCreator.ino    # Main firmware (setup/loop)
├── LCD_CONFIG.h           # I2C LCD 16x2 driver
├── THERMISTOR_CONFIG.h    # NTC thermistor (100K, Beta 3950)
├── RELAY_CONFIG.h         # Relay control (heater on/off)
└── PID_CONFIG.h           # PID temperature controller (P=4, I=0, D=22)
docs/
├── architecture.md        # System overview & data flow
├── api-reference.md       # Function-by-function API docs
├── hardware-setup.md      # Wiring, components, pinout
├── pid-tuning.md          # PID theory, tuning guide
└── workflow.md            # Boot sequence & control loop
```

## Hardware Requirements

| Component | Specification |
|-----------|---------------|
| Microcontroller | Arduino Uno / Nano |
| Display | 16x2 I2C LCD (0x27) |
| Temperature Sensor | NTC 100K Thermistor (Beta 3950) |
| Heater Control | Relay Module (pin 7, active LOW) |
| Heater | 220V/110V band heater |

## Author

**github.com/qppd**