# PID Tuning & Known Issues

## Current Tuning

| Parameter | Value | Effect |
|-----------|-------|--------|
| P (Proportional) | 4 | Immediate response to error |
| I (Integral) | 0 | Disabled — no steady-state correction |
| D (Derivative) | 22 | Strong damping to prevent overshoot |
| Setpoint | 245°C | PET extrusion temperature |

## PID Theory

The PID controller computes an output based on the error between the setpoint (245°C) and the current temperature:

```
output = P × error + I × ∫error·dt + D × d(error)/dt
```

With **I = 0**, the integral term is disabled. This means:
- The controller reacts proportionally to temperature error
- There is strong derivative damping to prevent oscillation
- **No steady-state error correction** — the temperature may never reach exactly 245°C

## Known Issues

### 1. computePID() — Relay Always OFF

**File:** `PID_CONFIG.h`, lines 21–25

```cpp
void computePID() {
  pid.Compute();
  if (output > 0) {
    operateRELAY(RELAY_1, HIGH);  // ← BUG: should be LOW
  } else {
    operateRELAY(RELAY_1, HIGH);  // ← BUG: should be LOW (heater OFF when cold)
  }
}
```

**Problem:** Both branches set the relay to `HIGH` (which means OFF for active-LOW relay). The heater **never turns on**. Additionally, when `output ≤ 0` (temperature above setpoint), the relay should turn OFF — but both branches are identical.

**Fix:**

```cpp
void computePID() {
  pid.Compute();
  if (output > 0) {
    operateRELAY(RELAY_1, true);   // true = OPENED → LOW → heater ON
  } else {
    operateRELAY(RELAY_1, false);  // false = CLOSED → HIGH → heater OFF
  }
}
```

### 2. thermistor — Wrong Variable in log()

**File:** `THERMISTOR_CONFIG.h`, line 28

```cpp
TEMPERATURE = CURRENT / RESISTANCE_REF;           // (R/Ro)
TEMPERATURE = log(CURRENT_TEMPERATURE);           // ← BUG: should be TEMPERATURE
```

**Problem:** Line 27 computes `TEMPERATURE = CURRENT / RESISTANCE_REF`, but line 28 calls `log(CURRENT_TEMPERATURE)` instead of `log(TEMPERATURE)`. `CURRENT_TEMPERATURE` is the **previous** reading (uninitialized on first call), so the log calculation uses the wrong value.

**Fix:**

```cpp
TEMPERATURE = CURRENT / RESISTANCE_REF;           // (R/Ro)
TEMPERATURE = log(TEMPERATURE);                  // ln(R/Ro) — use TEMPERATURE, not CURRENT_TEMPERATURE
```

### 3. No Integral Term (I=0)

With I=0, the controller has no memory of past errors. If the proportional+dervative terms settle below the setpoint, the controller will never correct. This is called **steady-state error**.

**Recommendation:** Set `I` to a small value (e.g., `I = 0.5`) and re-tune:

```
P = 4
I = 0.5   ← added
D = 22
```

Or use the Ziegler-Nichols tuning method:
1. Set I=0, D=0
2. Increase P until the system oscillates steadily
3. Note the ultimate gain (Ku) and oscillation period (Tu)
4. Apply: `P = 0.6×Ku`, `I = 2/Tu`, `D = Tu/8`

## Tuning Procedure

1. Upload firmware with I=0, small D
2. Open Serial Monitor (9600 baud)
3. Observe temperature readings
4. Gradually increase P until temperature approaches setpoint quickly
5. Add D to reduce overshoot
6. Add small I (0.1–1.0) to eliminate steady-state error
7. Fine-tune for your specific heater/thermistor combination

## Expected Behavior (After Fix)

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