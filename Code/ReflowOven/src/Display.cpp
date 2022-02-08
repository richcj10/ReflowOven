#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Display.h"
#include "Temperature.h"
#include "Version.h"
#include "Define.h"

char WiFiMode = 0;
char ReflowMode = 0;

char ScreenPage = 0;
char previousPage = 0;
char ButtonPressed = 0;
char ScreenAlive = 0;

void ScreenDataupdate();
char CheckScreenFB();
void WriteObject(uint16_t object, uint16_t index, uint16_t data);
char genieWriteStr(uint16_t index, char *string);

void DisplaySetup(){
  pinMode(DSPLYRSET, OUTPUT);
  digitalWrite(DSPLYRSET, HIGH);
  //Serial.begin(115200);
  delay(100);
  digitalWrite(DSPLYRSET, LOW);
  delay(3000);
}

void DisplayUpdate(){
  CheckScreenFB();
  ScreenDataupdate();
}

void ScreenDataupdate(){
  String Temp = String(TempRead(),2);
  int ScopeValue = map(TempRead(), 0, 300, 0, 100);
  switch (ScreenPage) {
    case 0:    // your hand is on the sensor
      if(previousPage != ScreenPage){  //Does it only need to run it once in this state, put the code here
        previousPage = ScreenPage;
      }
      //WriteObject(USERLED, 0, Led);
      WriteObject(SCOPE, 0, ScopeValue);
      genieWriteStr(0, (char*)Temp.c_str());
      break;
    case 1:    // your hand is close to the sensor
      break;
    case 2:    // your hand is a few inches from the sensor
      if(previousPage != ScreenPage){  //Does it only need to run it once in this state, put the code here
        previousPage = ScreenPage;
        genieWriteStr(1, "192.168.5.1/rLights.Camera.Action");
      }
      if(ButtonPressed == 5){
        //WiFi AP Mode
        ButtonPressed = 0;
        
      }
      break;
    case 3:    // your hand is nowhere near the sensor
      if(previousPage != ScreenPage){  //Does it only need to run it once in this state, put the code here
        previousPage = ScreenPage;
        genieWriteStr(2, "Reflow Profile");
      }
      break;
    case 4:    // your hand is nowhere near the sensor
      if(previousPage != ScreenPage){  //Does it only need to run it once in this state, put the code here
        previousPage = ScreenPage;
        genieWriteStr(3, (char *)String(VERSION).c_str());
        genieWriteStr(4, (char *)String(BUILD_TIMESTAMP).c_str());
      }
      break;
  }
}

void WriteObject(uint16_t object, uint16_t index, uint16_t data){
    uint16_t msb, lsb;
    uint8_t checksum;
    lsb = lowByte(data);
    msb = highByte(data);
    Serial.write(1);
    checksum  = 1;
    Serial.write(object);
    checksum ^= object;
    Serial.write(index);
    checksum ^= index;
    Serial.write(msb);
    checksum ^= msb;
    Serial.write(lsb);
    checksum ^= lsb;
    Serial.write(checksum);
}

char genieWriteStr(uint16_t index, char *string){
  char* p;
  uint8_t checksum;
  int len = strlen(string);
  if (len > 255)
      return -1;
  Serial.write(2);
  checksum = 2;
  Serial.write(index);
  checksum ^= index;
  Serial.write((unsigned char)len);
  checksum ^= len;
  for (p = string; *p; ++p) {
    Serial.write(*p);
    checksum ^= *p;
  }
  Serial.write(checksum);
  return 1;
}

char CheckScreenFB(){
  char SreenData[20];
  char Lenth;
  if(Serial.available() >= 2){
    Lenth = Serial.available();
    for(char k=0;k<Lenth;k++){
      SreenData[0] = Serial.read();
      if(SreenData[0] == 6){
        ScreenAlive = 1;
      }
      if(SreenData[0] == 7){
        SreenData[1] = Serial.read();
        if(SreenData[1] == 10){ //Fourm Comand
          ScreenPage = Serial.read();
        }
        else if(SreenData[1] == 6){ //Btn Comand
          ButtonPressed = Serial.read();
        }
        else if(SreenData[1] == 30){ //Spcl Button Command
          Serial.read();
          Serial.read();
          ReflowMode = Serial.read();
        }
      }
    }
  }
  return 1;
}

char GetGuiWiFi(){
  return WiFiMode;
}

char GetGuiReflow(){
  return ReflowMode;
}