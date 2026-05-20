## Control Loop (loop function)

```
┌─────────────────────────────────────────┐
│                                         │
│  ┌─────────────────────────────────┐    │
│  │ getTHERMISTORTemperature()      │    │
│  │ → Read analog pin A0            │    │
│  │ → Beta equation → Celsius       │    │
│  │ → Store in CURRENT_TEMPERATURE  │    │
│  │ → Return double                 │    │
│  └──────────────┬──────────────────┘    │
│                 │                        │
│  ┌──────────────▼──────────────────┐    │
│  │ Serial.print(temp)              │    │
│  │ → Debug output to serial        │    │
│  └──────────────┬──────────────────┘    │
│                 │                        │
│  ┌──────────────▼──────────────────┐    │
│  │ setLCDText(temp, 5, 1)          │    │
│  │ → Display on LCD row 1          │    │
│  │   Column 5: "245.00C"           │    │
│  └──────────────┬──────────────────┘    │
│                 │                        │
│  ┌──────────────▼──────────────────┐    │
│  │ computePID()                    │    │
│  │ → pid.Compute()                 │    │
│  │ → If output > 0: relay ON       │    │
│  │   If output ≤ 0: relay OFF      │    │
│  └──────────────┬──────────────────┘    │
│                 │                        │
│                 └────── loop back ───────┘
│                                         │
└─────────────────────────────────────────┘
```

## LCD Layout

```
┌──────────────────┐
│PID Control:245C  │  ← Row 0 (setpoint display)
│TEMP:245.00C      │  ← Row 1 (live reading at col 5)
└──────────────────┘
```

## Serial Output Format

```
Setting up LCD I2C pins!Success!
Setting up relay pins!Success!
Current Temperature(double):25.43C
Current Temperature(double):27.12C
Current Temperature(double):30.55C
...
```

## Timing

- **Splash screen:** 3 seconds (3 × 500ms delays)
- **Control loop:** No explicit delay — runs as fast as Arduino allows (~1ms per iteration)
- **Serial baud rate:** 9600

## State Variables

| Variable | Type | Module | Scope |
|----------|------|--------|-------|
| `CURRENT_TEMPERATURE` | `double` | THERMISTOR_CONFIG.h | Global — set by thermistor, read by PID |
| `output` | `double` | PID_CONFIG.h | Global — PID output (0–5000) |
| `SETPOINT` | `double` | PID_CONFIG.h | Global — 245°C target |
| `P`, `I`, `D` | `double` | PID_CONFIG.h | Global — tuning constants |

## What Happens When...

### Temperature rises above 245°C
1. PID error goes negative
2. `pid.Compute()` → `output` decreases → approaches 0
3. `computePID()` → relay turns OFF (HIGH)
4. Heater stops
5. Temperature drops
6. PID increases `output` → relay turns ON

### Temperature drops below 245°C
1. PID error goes positive
2. `pid.Compute()` → `output` increases
3. `computePID()` → relay turns ON (LOW)
4. Heater activates
5. Temperature rises

### At exactly 245°C
1. PID error = 0
2. With I=0.5, output may hold a small steady-state value
3. Relay pulses to maintain temperature
4. Any disturbance → PID reacts