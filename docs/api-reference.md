# API Reference

Complete function-by-function reference for all modules.

## FilamentCreator.ino — Main Firmware

### `setup()`

Initializes all subsystems in sequence.

```cpp
void setup()
```

**Execution order:**
1. `Serial.begin(9600)` — Start serial monitor
2. `initLCD()` — Initialize 16x2 I2C LCD
3. Splash screen → `setLCDText("Plastic Filament", 0, 0)` + `setLCDText("Maker", 0, 1)`, 3-second delay
4. `initRELAY()` — Configure relay pin
5. `initTHERMISTOR()` — Set analog reference
6. `initPID()` — Configure PID controller
7. Display setpoint → `"PID Control:245C"` + temperature placeholder

---

### `loop()`

Main control loop — runs continuously.

```cpp
void loop()
```

**Every cycle:**
1. `getTHERMISTORTemperature()` → reads sensor, returns `double`
2. Serial print current temperature
3. `setLCDText(temp, 5, 1)` → show on LCD (column 5, row 1)
4. `computePID()` → run PID calculation → adjusts relay

---

## LCD_CONFIG.h — I2C LCD Driver

**Hardware:** 16x2 I2C LCD at address `0x27`  
**Library:** `LiquidCrystal_I2C`

### `initLCD()`

```cpp
void initLCD()
```

Initializes the LCD and turns on the backlight. Prints status to serial.

---

### `clearLCD()`

```cpp
void clearLCD()
```

Clears all characters from the LCD.

---

### `setLCDText(text, x, y)`

Overloaded function — accepts multiple types:

```cpp
void setLCDText(String text, int x, int y)
void setLCDText(double value, int x, int y)
void setLCDText(float value, int x, int y)
void setLCDText(int value, int x, int y)
void setLCDText(char text, int x, int y)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `text` / `value` | String/double/float/int/char | Content to display |
| `x` | int | Column position (0–15) |
| `y` | int | Row position (0 or 1) |

---

## THERMISTOR_CONFIG.h — Temperature Sensor

**Hardware:** NTC 100K thermistor, Beta = 3950  
**Pin:** Analog `A0` (`THERMISTOR_PIN = 0`)  
**Reference:** External voltage (`analogReference(EXTERNAL)`)

### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `THERMISTOR_PIN` | 0 | Analog pin A0 |
| `RESISTANCE_REF` | 100000 | Reference resistance (100K at 25°C) |
| `TEMPERATURE_REF` | 25 | Reference temperature (°C) |
| `BETA` | 3950 | Beta coefficient for NTC curve |
| `RESISTANCE` | 100000 | Nominal resistance (100K) |

### Global Variables

| Variable | Type | Description |
|----------|------|-------------|
| `CURRENT_TEMPERATURE` | `double` | Latest temperature reading (°C) — shared with PID |

---

### `initTHERMISTOR()`

```cpp
void initTHERMISTOR()
```

Sets analog reference to `EXTERNAL`. Must be called before `getTHERMISTORTemperature()`.

---

### `getTHERMISTORTemperature()`

```cpp
double getTHERMISTORTemperature()
```

**Returns:** `double` — Temperature in degrees Celsius.

**Algorithm (Steinhart-Hart Beta method):**

```
1. Read analog voltage:  CURRENT = analogRead(THERMISTOR_PIN)
2. Convert to resistance: CURRENT = 1023 / CURRENT - 1
                          CURRENT = 100000 / CURRENT
3. Apply Beta equation:
   TEMP = CURRENT / RESISTANCE_REF        → R/Ro
   TEMP = ln(TEMP)                        → ln(R/Ro)
   TEMP = TEMP / BETA                     → (1/B) * ln(R/Ro)
   TEMP = TEMP + 1/(TEMPERATURE_REF+273.15) → + 1/To
   TEMP = 1 / TEMP                        → Kelvin
   TEMP = TEMP - 273.15                   → Celsius
4. Store in CURRENT_TEMPERATURE
5. Return
```

> **Note:** Line 28 has a bug — it uses `CURRENT_TEMPERATURE` instead of `TEMPERATURE` in the `log()` call. See [`pid-tuning.md`](./pid-tuning.md#thermistor-bug).

---

## RELAY_CONFIG.h — Relay / Heater Control

**Hardware:** Relay module on pin 7 (`RELAY_1 = 7`)  
**Logic:** Active LOW — `LOW` = relay ON, `HIGH` = relay OFF

### `initRELAY()`

```cpp
void initRELAY()
```

Sets `RELAY_1` (pin 7) as `OUTPUT`. Prints status to serial.

---

### `operateRELAY(RELAY, OPENED)`

```cpp
void operateRELAY(int RELAY, boolean OPENED)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `RELAY` | int | Relay pin number (currently only `RELAY_1 = 7`) |
| `OPENED` | boolean | `true` → relay ON (LOW), `false` → relay OFF (HIGH) |

---

## PID_CONFIG.h — PID Temperature Controller

**Library:** `PID_v1` by Brett Beauregard  
**Tuning:** P=4, I=0, D=22  
**Setpoint:** 245°C

### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `P` | 4 | Proportional gain |
| `I` | 0 | Integral gain (disabled) |
| `D` | 22 | Derivative gain |
| `SETPOINT` | 245 | Target temperature (°C) |

---

### `initPID()`

```cpp
void initPID()
```

Configures the PID controller:
- Mode: `AUTOMATIC`
- Output limits: `0` to `5000`

---

### `computePID()`

```cpp
void computePID()
```

Runs `pid.Compute()` and controls the relay based on output.

```cpp
if (output > 0) {
    operateRELAY(RELAY_1, HIGH);  // BUG: should be LOW
} else {
    operateRELAY(RELAY_1, HIGH);  // Always HIGH
}
```

> **Bug:** Both branches set the relay to `HIGH` (OFF). The heater is never turned ON. With I=0 (no integral), the PID cannot correct. See [`pid-tuning.md`](./pid-tuning.md) for the fix.