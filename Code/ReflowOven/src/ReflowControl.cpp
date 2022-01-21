#include "ReflowControl.h"
#include "Define.h"
#include "Profiles.h"
#include "Temperature.h"
#include <Arduino.h>
#include "TimerInterrupt_Generic.h"

float P_KE = 1.0;
float I_KE = 1.0;
float D_KE = 1.0;
float IntError = 0;
float DivError = 0;

struct ReflowConfig {
  int PreheatTemp = 0;
  int PreheatRamp = 0;
  unsigned long PreheatDwel = 0;
  int FlowTemp = 0;
  unsigned long FlowDwel = 0;
  int FlowRamp = 0;
  int CoolRamp = 0;
  int CoolStop = 0;
};

enum ReflowStates {
    off = 0,
    verifyscript = 1,
    starting = 2,
    Warmupramp = 3,
    preheat = 4,
    dwelramp = 5,
    dwel = 6,
    coolramp = 7,
    end = 8,
    NUM_STATES = 9
};

ESP8266Timer ITimer;
ReflowConfig ActiveReflowData;

float PIDLoop(float CurrentTemp, float SetTemp){
  float Perror = 0.0;
  float Ierror = 0.0;
  float Derror = 0.0;
  float Diff = 0.0;
  Diff = CurrentTemp-SetTemp;
  IntError += Diff;
  Perror = P_KE*(Diff);
  if (IntError > 2000){
    IntError = 2000;
  }
  else if (IntError < 0){
    IntError = 0;
  }
  Ierror = I_KE*(IntError);
  Derror = D_KE*(DivError - Diff);
  DivError = Diff;
  float PID = (Perror+Ierror+Derror);
  if(PID < 0){
    PID = 0;
  }
  if(PID > 8){
    PID = 8;
  }
  return PID;
  //Serial.print(PID);
}

volatile int Control = 0;
volatile int count = 0;

void IRAM_ATTR TimerHandler(){
  count++;
  if(count <= Control){
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

enum ReflowStates RunProfileStat = off;  
enum ReflowStates previousState = NUM_STATES;

unsigned long TimeStamp = 0;
float SetTemp = 0;

void RunProfile(int ProfileType){
  switch (RunProfileStat) {
    case off:
      // statements
      RunProfileStat = verifyscript;
      break;
    case verifyscript:
      // statements
      RunProfileStat = starting;
      break;
    case starting:
      LoadProfileData(1);
      PrintReflowDataSet();
      StartISR();
      RunProfileStat = Warmupramp;
      // statements
      break;
    case Warmupramp:
      if(previousState != RunProfileStat){  //Does it only need to run it once in this state, put the code here
        previousState = RunProfileStat;
        SetTemp = TempRead();
      }
      if(SetTemp < ActiveReflowData.PreheatTemp){
        SetTemp += ActiveReflowData.PreheatRamp;
      }
      if((TempRead() < (ActiveReflowData.PreheatTemp+DELTA)) || (TempRead() > (ActiveReflowData.PreheatTemp-DELTA))){
        //Dwell Time 
        RunProfileStat = preheat;
      }
      break;
    case preheat:
      if(previousState != RunProfileStat){  //Does it only need to run it once in this state, put the code here
        previousState = RunProfileStat;
        TimeStamp = millis();
      }
      if((millis()-TimeStamp) > ActiveReflowData.PreheatDwel){
        //Ramp to Dwel 
        RunProfileStat = dwelramp;
      }
      break;
    case dwelramp:
      if(SetTemp < ActiveReflowData.FlowTemp){
        SetTemp += ActiveReflowData.FlowRamp;
      }
      if((TempRead() < (ActiveReflowData.FlowTemp+DELTA)) || (TempRead() > (ActiveReflowData.FlowTemp-DELTA))){
        //Dwell Time 
        RunProfileStat = dwel;
      }
      break;
    case dwel:
      if(previousState != RunProfileStat){  //Does it only need to run it once in this state, put the code here
        previousState = RunProfileStat;
        TimeStamp = millis();
      }
      if((millis()-TimeStamp) > ActiveReflowData.FlowDwel){
        //Ramp to Dwel 
        RunProfileStat = dwelramp;
      }
      break;
    case coolramp:
      // statements
      StopISR();
      RunProfileStat = end;
      break;
    case end:
      if(TempRead() < 30){
        //Profile Over, set end bit
        RunProfileStat = off;
      }
      // statements
      break;
    default:
      // statements
      break;
  }
  float Result = PIDLoop(TempRead(),SetTemp);
  Control = (int)Result;
  Serial.println("The current State = " + String(RunProfileStat));
  Serial.println("Current Set Temp = " + String(SetTemp)+" ^C");
  Serial.println("Current Temp = " + String(TempRead())+" ^C");
  Serial.println("PID OUTPUT = " + String(Result));
}

void SetProfileValue(char DataSet, int Value){
  switch (DataSet) {
    case PREHEATTMP:
      ActiveReflowData.PreheatTemp = Value;
      break;
    case PREHEATRMP:
      ActiveReflowData.PreheatRamp = Value;
      break;
    case PREHEATDWL:
      ActiveReflowData.PreheatDwel = Value;
      break;
    case FLWTMP:
      ActiveReflowData.FlowTemp = Value;
      break;
    case FLWDWL:
      ActiveReflowData.FlowDwel = Value;
      break;
    case FLWRMP:
      ActiveReflowData.FlowRamp = Value;
      break;
    case COOLRMP:
      ActiveReflowData.CoolRamp = Value;
      break;
    case COOLSTOP:
      ActiveReflowData.CoolStop = Value;
      //PrintReflowDataSet();
      break;
    default:
      Serial.println("NP");
      break;
  }
}

void PrintReflowDataSet(){
  Serial.println("Preheat Temp (C) = " + String(ActiveReflowData.PreheatTemp));
  Serial.println("Preheat Ramp (C/S) = " + String(ActiveReflowData.PreheatRamp));
  Serial.println("Preheat Dwel (S) = " + String(ActiveReflowData.PreheatDwel));
  Serial.println("Flow Temp (C) = " + String(ActiveReflowData.FlowTemp));
  Serial.println("Flow Dwel (S) = " + String(ActiveReflowData.FlowDwel));
  Serial.println("Flow Ramp (C/S)= " + String(ActiveReflowData.FlowRamp));
  Serial.println("Cool Ramp (C/S)= " + String(ActiveReflowData.CoolRamp));
  Serial.println("Cool Stop (C) = " + String(ActiveReflowData.CoolStop));
}

void ReflowStop(){
  StopISR();
  RunProfileStat = off;
}