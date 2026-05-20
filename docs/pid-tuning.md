# PID Tuning

## Current Tuning

| Parameter | Value | Effect |
|-----------|-------|--------|
| P (Proportional) | 4 | Immediate response to error |
| I (Integral) | 0.5 | Eliminates steady-state error |
| D (Derivative) | 22 | Strong damping to prevent overshoot |
| Setpoint | 245°C | PET extrusion temperature |

## PID Theory

The PID controller computes an output based on the error between the setpoint (245°C) and the current temperature:

```
output = P × error + I × ∫error·dt + D × d(error)/dt
```

- **P=4** — Proportional: reacts to current error. Higher = faster response, but too high causes oscillation.
- **I=0.5** — Integral: accumulates past errors to eliminate steady-state offset. Too high causes overshoot.
- **D=22** — Derivative: reacts to rate of change, anticipating future error. Dampens oscillation and reduces overshoot.

## Tuning Procedure

If you need to re-tune for a different heater or thermistor:

### Ziegler-Nichols Method

1. Set `I=0`, `D=0`
2. Increase `P` until the system oscillates steadily (constant amplitude)
3. Note the **ultimate gain** (Ku) — the P value where oscillation begins
4. Note the **oscillation period** (Tu) in seconds
5. Apply:
   ```
   P = 0.6 × Ku
   I = 2 / Tu
   D = Tu / 8
   ```

### Manual Tuning

1. Upload firmware with `I=0`, small `D`
2. Open Serial Monitor (9600 baud)
3. Gradually increase `P` until temperature rises quickly to near setpoint
4. Increase `D` to reduce overshoot / ringing
5. Add small `I` (0.1–1.0) to eliminate any remaining steady-state error
6. Fine-tune for your specific heater/thermistor combination

## Relay Logic

The relay is **active LOW** — writing `LOW` to the pin energizes the relay (heater ON).

`operateRELAY(RELAY, OPENED)`:
- `operateRELAY(RELAY_1, true)` → `digitalWrite(RELAY_1, LOW)` → **heater ON**
- `operateRELAY(RELAY_1, false)` → `digitalWrite(RELAY_1, HIGH)` → **heater OFF**

## Expected Behavior

```
time  temp   output  relay
0s    25°C   5000    ON   ← cold start, full power
30s   100°C  4000    ON
60s   180°C  2500    ON
90s   220°C  800     ON
120s  240°C  200     ON   ← approaching setpoint
150s  245°C  0       OFF  ← at setpoint
160s  243°C  300     ON   ← slight dip, quick correction
180s  245°C  0       OFF  ← stable
```