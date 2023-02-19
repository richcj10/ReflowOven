#ifndef PROFILES_H
#define  PROFILES_H

#include <Arduino.h>

void ProfileSetup();
void listDir(const char * dirname);
bool saveConfig(String SSID, String Password);
char SaveProfile(String Name, float PreheatTemp, float PreheatRamp, float PreheatDwel, float FlowTemp, float FlowDwel, float FlowRamp, float CoolRamp, float CoolOff);
String GetSSID();
String GetSSIDPassword();
char WifiConfigStatus();
void LoadProfileName();
unsigned char ProfileCount(char i);
unsigned char LastProfileRead();
void SetLastProfileRead(unsigned char i);
void LoadProfileData(char ProfileNumber);
String GetProfileNames();
String GetProfileName(unsigned char j);

#endif