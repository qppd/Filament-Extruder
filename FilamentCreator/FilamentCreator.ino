#include "LCD_CONFIG.h"
#include "THERMISTOR_CONFIG.h"
#include "RELAY_CONFIG.h"
#include "PID_CONFIG.h"

void setup() {

  Serial.begin(9600);

  initLCD();

  setLCDText("Plastic Filament", 0, 0);
  delay(500);
  setLCDText("     Maker      ", 0, 1);
  delay(3000);
  clearLCD();


  initRELAY();
  initTHERMISTOR();
  initPID();


  setLCDText("PID Control:245C", 0, 0);
  setLCDText("TEMP:", 0, 1);
}



void loop() {

  double temp  = getTHERMISTORTemperature();

  Serial.print("Current Temperature(double):");
  Serial.print(temp);
  Serial.println("C");
  setLCDText(String(temp) + "C" , 5, 1);


  computePID();

  
}
