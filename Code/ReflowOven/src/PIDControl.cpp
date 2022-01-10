#include "PIDControl.h"
#include "Temperature.h"
#include <Arduino.h>
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#include "TimerInterrupt_Generic.h"

float P_KE = 1.0;
float I_KE = 1.0;
float D_KE = 1.0;
float IntError = 0;
float DivError = 0;
#define TIMER_INTERVAL_MS        250
ESP8266Timer ITimer;

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
  //Serial.print(PID);
}

volatile int Rndnumber = 0;
volatile int count = 0;

void IRAM_ATTR TimerHandler(){
  count++;
  if(count <= Rndnumber){
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if(count > 8){
    count = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void StartISR(){
    if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    Serial.print(F("Starting  ITimer OK, millis() = "));
  }
  else
    Serial.println(F("Can't set ITimer correctly. Select another freq. or interval"));
}

void StopISR(){
    ITimer.detachInterrupt();
    digitalWrite(LED_BUILTIN, LOW);
}