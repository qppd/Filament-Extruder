int THERMISTOR_PIN = 0;

int RESISTANCE_REF = 100000;
int TEMPERATURE_REF = 25;

int BETA = 3950;
int RESISTANCE = 100000;

float CURRENT;

double CURRENT_TEMPERATURE;

void initTHERMISTOR() {
  analogReference(EXTERNAL);
}

double getTHERMISTORTemperature() {

  float TEMPERATURE = 0;
  float CURRENT = 0;

  CURRENT = analogRead(THERMISTOR_PIN);
  CURRENT = 1023 / CURRENT - 1;
  CURRENT = RESISTANCE / CURRENT;

  //Calculation of temperature according to the relation for the beta factor
  TEMPERATURE = CURRENT / RESISTANCE_REF;           // (R/Ro)
  TEMPERATURE = log(TEMPERATURE);                   // ln(R/Ro)
  TEMPERATURE /= BETA;                              // 1/B * ln(R/Ro)
  TEMPERATURE += 1.0 / (TEMPERATURE_REF + 273.15);  // + (1/To)
  TEMPERATURE = 1.0 / TEMPERATURE;                  // The inverted value
  TEMPERATURE -= 273.15;                            // Convert from Kelvin to degrees Celsius

  CURRENT_TEMPERATURE = TEMPERATURE;
  return CURRENT_TEMPERATURE;
}
