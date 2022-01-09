#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Temperature.h"
#include "Profiles.h"
#include "Display.h"
#include "WebService.h"

// Replace with your network credentials
const char* ssid = "Lights.Camera.Action";
const char* password = "RR58fa!8";

char WiFiStartupTimeout = 0;
char WiFiTimeout = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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
      Serial.println("Connecting to WiFi..");
      WiFiStartupTimeout = WiFiStartupTimeout +1;
      if(WiFiStartupTimeout > 10){
        WiFiTimeout = 1;
        break;
      }
    }
    if(WiFiTimeout == 1){
      Serial.println("Wifi Timeout! - AP Mode");
      WiFi.softAP("ReflowOven");
    }
  }
  Serial.println("Ready");



  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  WebserviceBegin();
  Serial.println("Weppage Started!");
}

unsigned long Time = millis();
int updateTime = 1000;

void loop() {
  if((Time + updateTime) < millis()){ //update every 20ms without blocking!
    TempRead();
    Time = millis();
    Serial.print("Free Heap: ");
    Serial.println(ESP.getFreeHeap());
    //Serial.println("ReadTemp");
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
  Serial.print(PID);
}

