# Architecture & Data Flow

## System Overview

The Filament Extruder is a single-purpose embedded controller running on an Arduino microcontroller. Its job is to maintain a precise 245°C temperature at the extruder nozzle while recycling PET bottles into 3D printing filament.

## Modular Design

The firmware is split into 5 files — one `.ino` main file and four `.h` config modules. Each module encapsulates one hardware subsystem:

```
┌─────────────────────────────────────────────────┐
│              FilamentCreator.ino                 │
│        setup() → init all modules               │
│        loop()  → read temp → PID → relay        │
├─────────────────────────────────────────────────┤
│  LCD_CONFIG.h     │  THERMISTOR_CONFIG.h         │
│  - initLCD()      │  - initTHERMISTOR()          │
│  - setLCDText()   │  - getTHERMISTORTemperature()│
│  - clearLCD()     │  - Steinhart-Hart (Beta)     │
├───────────────────┼──────────────────────────────┤
│  RELAY_CONFIG.h   │  PID_CONFIG.h                │
│  - initRELAY()    │  - initPID()                 │
│  - operateRELAY() │  - computePID()              │
└───────────────────┴──────────────────────────────┘
```

## Data Flow

```
          ┌──────────────┐
          │  Thermistor   │  (analog pin A0)
          │  NTC 100K     │
          └──────┬───────┘
                 │ Voltage
                 ▼
          ┌──────────────┐
          │ getTHERMISTOR │  Steinhart-Hart Beta equation
          │ Temperature() │  → Celsius value
          └──────┬───────┘
                 │ double CURRENT_TEMPERATURE
                 ▼
          ┌──────────────┐
          │   PID         │  P=4, I=0.5, D=22
          │   Controller   │  SETPOINT = 245°C
          └──────┬───────┘
                 │ double output (0–5000)
                 ▼
          ┌──────────────┐
          │   Relay       │  output > 0 → RELAY_1 ON
          │   (pin 7)     │  output ≤ 0 → RELAY_1 OFF
          │   active LOW  │
          └──────┬───────┘
                 │
                 ▼
          ┌──────────────┐
          │  Band Heater  │  220V/110V AC
          │  (Extruder)   │
          └──────────────┘
```

## Control Loop (loop function)

```
1. Read thermistor temperature → CURRENT_TEMPERATURE (double)
2. Display temperature on LCD ("245C")
3. Run PID.compute() → adjusts `output` variable
4. If output > 0 → relay ON (heater active)
   If output ≤ 0 → relay OFF
5. Repeat
```

## Boot Sequence (setup function)

```
1. Serial.begin(9600)
2. initLCD()          → Initialize I2C LCD, backlight on
3. Splash screen:     "Plastic Filament" / "Maker" (3 seconds)
4. initRELAY()        → Set pin 7 as OUTPUT
5. initTHERMISTOR()   → Set analog reference to EXTERNAL
6. initPID()          → Set PID to AUTOMATIC, output limits 0–5000
7. Display setpoint:  "PID Control:245C"
```

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| `LiquidCrystal_I2C` | any | I2C 16x2 LCD driver |
| `PID_v1` | Brett Beauregard | PID control algorithm |