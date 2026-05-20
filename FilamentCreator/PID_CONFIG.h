#include <PID_v1.h>

double P = 4;
double I = 0.5;
double D = 22;

double SETPOINT = 245;
double output = 0;

PID pid(&CURRENT_TEMPERATURE, &output, &SETPOINT, P, I, D, DIRECT);

void initPID() {

  pid.SetMode(AUTOMATIC);
  pid.SetOutputLimits(0, 5000);  
}


void computePID() {
  pid.Compute();
  if (output > 0) {
    operateRELAY(RELAY_1, true);  // true = OPENED → LOW → heater ON
  } else {
    operateRELAY(RELAY_1, false); // false → HIGH → heater OFF
  }
}