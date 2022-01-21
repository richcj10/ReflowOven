#include <Arduino.h>
#include "Define.h"
#include "Version.h"
#include <ESP8266WiFi.h>
#include "Temperature.h"
#include "Profiles.h"
#include "Display.h"
#include "WebService.h"
#include "Debug.h"
#include "ReflowControl.h"

// Replace with your network credentials
const char* ssid = "Lights.Camera.Action";
const char* password = "RR58fa!8";

char WiFiStartupTimeout = 0;
char WiFiTimeout = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(5,OUTPUT);
  DebugSetup(0);
  Serial.begin(115200);
  //DisplaySetup();
  Serial1.println("Boot");
  TempSetup();
  ProfileSetup();
  if(WifiConfigStatus() == 0){
      WiFi.softAP("ReflowOven");
  }
  else{
    WiFi.begin(ssid, password);
    //WiFi.begin(GetSSID().c_str(), GetSSIDPassword().c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println(".");
      WiFiStartupTimeout = WiFiStartupTimeout +1;
      if(WiFiStartupTimeout > 10){
        WiFiTimeout = 1;
        break;
      }
    }
    if(WiFiTimeout == 1){
      Serial.println("AP Mode");
      WiFi.softAP("ReflowOven");
    }
  }
  //Serial.println("Ready");

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  WebserviceBegin();
  //Serial.println("Weppage Started!");
  Serial.println("Project version: " + String(VERSION));
  Serial.println("Build timestamp:" + String(BUILD_TIMESTAMP));
  LoadProfileData(1);
  GetProfileNames();
}


unsigned long ReflowTime = millis();
unsigned long UxTime = millis();

char OvenState = 0;

void loop() {
  if((ReflowTime + OVENCONTROLLOOP) < millis()){ //update every 20ms without blocking!
    //TempRead();
    ReflowTime = millis();
    if(OvenState == 1){
      RunProfile(1);
    }
    if(OvenState == 0){
      ReflowStop();
    }
  }
  if((UxTime + UXCONTROLLOOP) < millis()){ //update every 20ms without blocking!
    UxTime = millis();
    //DisplayUpdate();
  }
  delay(1000);
  digitalWrite(5,1);
  delay(1000);
  digitalWrite(5,0);
}


