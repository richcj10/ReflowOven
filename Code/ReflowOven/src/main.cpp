#include <Arduino.h>
#include "Temperature.h"
#include "Display.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

unsigned long Time = millis();
int updateTime = 20;

void loop() {
  if((Time + updateTime) < millis()){ //update every 20ms without blocking!
    TempRead();
    Time = millis();
  }
}

float P_KE = 1.0;
float I_KE = 1.0;
float D_KE = 1.0;
float IntError = 0;
float DivError = 0;

void PIDLoop(float CurrentTemp, float SetTemp){
  float Perror = 0.0;
  float Ierror = 0.0;
  float Derror = 0.0;
  float Diff = 0.0;
  Diff = CurrentTemp-SetTemp;
  IntError += Diff;
  Perror = P_KE*(Diff);
  if (IntError > 200){
    IntError = 200;
  }
  else if (IntError < 0){
    IntError = 0;
  }
  Ierror = I_KE*(IntError);
  Derror = D_KE*(DivError - Diff);
  DivError = Diff;
  float PID = (Perror+Ierror+Derror);
}