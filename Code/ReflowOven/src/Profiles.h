#ifndef PROFILES_H
#define  PROFILES_H

#include <Arduino.h>

void ProfileSetup();
void listDir(const char * dirname);
bool saveConfig(String SSID, String Password);
String GetSSID();
String GetSSIDPassword();
char WifiConfigStatus();

#endif