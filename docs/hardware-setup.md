# Hardware Setup

## Components List

| Component | Specification | Quantity |
|-----------|---------------|----------|
| Microcontroller | Arduino Uno R3 (or Nano) | 1 |
| LCD Display | 16×2 Character LCD with I2C backpack | 1 |
| Thermistor | NTC 100K Ohm, Beta 3950 | 1 |
| Pull-down Resistor | 100K Ohm (for thermistor voltage divider) | 1 |
| Relay Module | 5V single-channel (active LOW) | 1 |
| Band Heater | 220V/110V AC, suitable for extruder barrel | 1 |
| Power Supply | 12V 5A (Arduino) + mains for heater | 1 |
| Connecting Wires | Jumper wires (M-M, M-F) | ~15 |

## Wiring Diagram

```
                    ARDUINO UNO
                 ┌──────────────┐
                 │              │
    I2C LCD ────┤ A4 (SDA)     │
    (0x27)  ────┤ A5 (SCL)     │
                 │              │
  Thermistor ────┤ A0           │
  (NTC 100K)     │              │
     GND ────────┤ GND          │
                 │              │
    Relay  ──────┤ D7           │
   (IN pin)      │              │
                 └──────────────┘

            RELAY MODULE
         ┌─────────────────┐
  D7 ───┤ IN               │
        │          NO ─────┼─── Band Heater ─── Mains Live
        │          COM ────┼─── Mains Live
        │          NC      │
        │    VCC ──────────┼─── 5V (Arduino)
        │    GND ──────────┼─── GND (Arduino)
         └─────────────────┘

        THERMISTOR VOLTAGE DIVIDER
           5V ───┬─── 100K ───┬─── A0 (analog read)
                 │            │
                 │         NTC 100K
                 │            │
                GND          GND
```

## Pin Assignments

| Function | Arduino Pin | Notes |
|----------|-------------|-------|
| Thermistor input | `A0` | Analog, voltage divider circuit |
| Relay control | `D7` | Digital, active LOW |
| I2C SDA | `A4` | LCD data line |
| I2C SCL | `A5` | LCD clock line |

## I2C LCD

- **Address:** `0x27`
- **Size:** 16 columns × 2 rows
- **Backlight:** Enabled on startup
- **Library:** `LiquidCrystal_I2C`

If the display doesn't work, scan the I2C address with an I2C scanner sketch — it may be `0x3F` on some modules.

## Thermistor

- **Type:** NTC (Negative Temperature Coefficient)
- **Resistance at 25°C:** 100,000 Ω (100K)
- **Beta coefficient:** 3950
- **Wiring:** Voltage divider with 100K pull-down resistor to GND
- **Reference:** External analog reference (`analogReference(EXTERNAL)`)

The Beta equation used in `getTHERMISTORTemperature()` converts resistance to temperature:

```
1/T = 1/T₀ + (1/β) × ln(R/R₀)
```

Where T₀ = 25°C (298.15K), R₀ = 100KΩ, β = 3950.

## Relay Module

- **Type:** Active LOW (relay ON when input pin is LOW)
- **Operation:** `digitalWrite(pin, LOW)` → relay closed (heater ON)
- **Operation:** `digitalWrite(pin, HIGH)` → relay open (heater OFF)
- **The `operateRELAY(RELAY, OPENED)` function handles the inversion:** when `OPENED=true`, it writes `LOW`

## Safety Notes

- ⚠️ The relay switches **mains voltage** — ensure proper insulation
- ⚠️ The band heater can reach **300°C+** — keep away from flammable materials
- ⚠️ Always test with low voltage before connecting mains
- ⚠️ Add a thermal fuse in series with the heater as a safety cut-off