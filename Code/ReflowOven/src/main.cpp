#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Temperature.h"
#include "Profiles.h"
#include "Display.h"
#include "WebService.h"
#include "Debug.h"

// Replace with your network credentials
const char* ssid = "Lights.Camera.Action";
const char* password = "RR58fa!8";

char WiFiStartupTimeout = 0;
char WiFiTimeout = 0;

void setup() {
  // put your setup code here, to run once:
  DebugSetup(0);
  Serial.begin(115200);
  //Serial1.println("Boot");
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
}

unsigned long Time = millis();
int updateTime = 1000;

void loop() {
  if((Time + updateTime) < millis()){ //update every 20ms without blocking!
    //TempRead();
    Time = millis();
    //Serial.print("Free Heap: ");
    //Serial.println(ESP.getFreeHeap());
    //Serial.println("ReadTemp");
    //PIDLoop(float CurrentTemp, float SetTemp);
  }
}


