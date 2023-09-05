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
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <spi_flash.h>

// Replace with your network credentials
const char* ssid = "JohnsonCamper";
const char* password = "RR58fa!8";

String WiFi_hostname = "ReflowOven";


char WiFiStartupTimeout = 0;
char WiFiTimeout = 0;

void setup() {
  pinMode(HEATERIO, OUTPUT);
  digitalWrite(HEATERIO, LOW);
  DisplayReset();
  // put your setup code here, to run once:
  pinMode(5,OUTPUT);
  DebugSetup(0);
  Serial.begin(115200);
  Serial.println("Boot");
  TempSetup();
  ProfileSetup();
  if(WifiConfigStatus() == 0){
       WiFi.softAP("ReflowOven");
  }
  else{
    WiFi.hostname(WiFi_hostname.c_str());
    WiFi.begin(ssid, password);
    //WiFi.begin(GetSSID().c_str(), GetSSIDPassword().c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(100); //1000
      Serial.println(".");
      WiFiStartupTimeout = WiFiStartupTimeout +1;
      if(WiFiStartupTimeout > 50){
        WiFiTimeout = 1;
        break;
      }
    }
  }
  if(WiFiTimeout == 1){
      //Serial.println("AP Mode");
      WiFi.softAP("ReflowOven");
  }
  Serial.println("Ready");
  //SetWifiConnect(1);
  // // Print ESP32 Local IP Address
  //Serial.println(WiFi.localIP());
  //Serial.println(WiFi.hostname());
  //WebserviceBegin();
  Serial.println("Weppage Started!");
  Serial.println("Project version: " + String(VERSION));
  Serial.println("Build timestamp:" + String(BUILD_TIMESTAMP));
  //SetLastProfileRead(1);
  // //GetProfileNames();
  //Serial.println(WiFi.localIP());

  //ArduinoOTA.onStart([]() {
  //  String type;
  //  if (ArduinoOTA.getCommand() == U_FLASH) {
  //    type = "sketch";
  //  } else { // U_FS
  //    type = "filesystem";
  //  }
  //
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
  //  Serial.println("Start updating " + type);
  //});
  //ArduinoOTA.onEnd([]() {
    //Serial.println("\nEnd");
  //});
  //ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  //});
  //ArduinoOTA.onError([](ota_error_t error) {
    // Serial.printf("Error[%u]: ", error);
    // if (error == OTA_AUTH_ERROR) {
    //   Serial.println("Auth Failed");
    // } else if (error == OTA_BEGIN_ERROR) {
    //   Serial.println("Begin Failed");
    // } else if (error == OTA_CONNECT_ERROR) {
    //   Serial.println("Connect Failed");
    // } else if (error == OTA_RECEIVE_ERROR) {
    //   Serial.println("Receive Failed");
    // } else if (error == OTA_END_ERROR) {
    //   Serial.println("End Failed");
    // }
  //});

  // Start OTA server.
  
  
  
  //DisplaySetup();
  //ArduinoOTA.setHostname(IPHostname);
  //ArduinoOTA.begin();
  PrintReflowDataSet();
}

unsigned long ReflowTime = millis();
unsigned long UxTime = millis();
unsigned long ReadTime = millis();

char OvenState = 0;

char kj = 0;

void loop() {
  //ArduinoOTA.handle();
  if((ReflowTime + OVEN_CONTROL_LOOP_MS) < millis()){ //update every OVEN_CONTROL_LOOP_MS without blocking!
    ReflowTime = millis();
    RunProfile();
    //Serial.println(1, DEC);
    //if(kj >= ProfileCount(0)){
    //  kj = 0;
    //}
    //Serial.println(GetProfileName(kj)); 
    //LoadProfileData(kj);
    //kj = kj +1;
    //Serial.println(kj, DEC);
  }
  if((UxTime + UX_CONTROL_LOOP_MS) < millis()){ //update every UX_CONTROL_LOOP_MS without blocking!
    UxTime = millis();
    //DisplayUpdate();
  }
  if((ReadTime + SAMPLE_INTERVAL_MS) < millis()){ //update every UX_CONTROL_LOOP_MS without blocking!
    ReadTime = millis();
    TempRead();
  }
}


