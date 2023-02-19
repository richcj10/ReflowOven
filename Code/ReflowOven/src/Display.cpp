#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Display.h"
#include "Temperature.h"
#include "Version.h"
#include "Define.h"
#include <WebSerial.h>
#include "ReflowControl.h"
#include "Profiles.h"

char IP[] = "xxx.xxx.xxx.xxx"; 
char SSID[25];
char buffer[20];

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
unsigned char RequestPage();

void DisplaySetup(){

  //Serial.begin(115200);
  delay(100);
  digitalWrite(RESETLINE, LOW);
  delay(3000);
}

void DisplayReset(){
  pinMode(RESETLINE, OUTPUT);
  digitalWrite(RESETLINE, HIGH);
}

void DisplayUpdate(){
  CheckScreenFB();
  ScreenDataupdate();
}

String IpAddressToString(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}

void ScreenDataupdate(){
  String Temp = String(TempRead(),2);
  int ScopeValue = map(TempRead(), 0, 300, 0, 100);
  RequestPage();
  switch (ScreenPage) {
    case 0: 
      if(previousPage != ScreenPage){ 
        previousPage = ScreenPage;
        unsigned char Prof = LastProfileRead();
        GetProfileName(Prof).toCharArray(buffer, 20);
        WebSerial.println("Setting Main Disp: ");
        WebSerial.println(Prof);
        genieWriteStr(7, buffer);
      }
      //WriteObject(USERLED, 0, GetHeaterState());
     //WriteObject(SCOPE, 0, ScopeValue);
      //genieWriteStr(0, (char*)Temp.c_str());
      break;
    case 1:   
      break;
    case 2:  
      if(previousPage != ScreenPage){  
        previousPage = ScreenPage;
        WiFi.SSID().toCharArray(SSID, WiFi.SSID().length());
        genieWriteStr(1,SSID);
        IPAddress ip = WiFi.localIP();
        ip.toString().toCharArray(IP, 16);
        genieWriteStr(5,IP);
        genieWriteStr(6, (char *)IPHostname);
      }
      if(ButtonPressed == 5){
        ButtonPressed = 0;
        
      }
      break;
    case 3:   
      if(previousPage != ScreenPage){  
        previousPage = ScreenPage;
        WebSerial.println("CNG Scrn");
        //genieWriteStr(2, "Reflow Profile");
      }
         if(ButtonPressed == 10){
          WebSerial.println("Scrn Up");
          ButtonPressed = 0;
          unsigned char MaxProf = ProfileCount(0);
          unsigned char Prof = LastProfileRead();
          if(Prof != MaxProf){
            Prof = Prof + 1;
            WebSerial.println(Prof);
            SetLastProfileRead(Prof);
            GetProfileName(Prof).toCharArray(buffer, 20);
            genieWriteStr(2, buffer);
          }
         }
         if(ButtonPressed == 9){
          WebSerial.println("Scrn Dn");
          ButtonPressed = 0;
          unsigned char MaxProf = ProfileCount(0);
          unsigned char Prof = LastProfileRead();
          if(Prof != 0){
            Prof = Prof - 1;
            WebSerial.println(Prof);
            SetLastProfileRead(Prof);
            GetProfileName(Prof).toCharArray(buffer, 20);
            genieWriteStr(2, buffer);
          }
         }
      break;
    case 4:  
      if(previousPage != ScreenPage){ 
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
  unsigned char Lenth;
  unsigned char pos = 0;
  if(Serial.available() >= 2){
    Lenth = Serial.available();
    for(unsigned char k=0;k<Lenth;k++){
      SreenData[k] = Serial.read();
      //WebSerial.print(SreenData[k]);
    }
    WebSerial.print(SreenData[pos]);
    if(SreenData[pos] == 7){
      WebSerial.println("Pct");
      //SreenData[1] = Serial.read();
      //WebSerial.println(SreenData[1]);
      pos = pos + 1;
      if(SreenData[pos] == 10){ //Fourm Comand
        pos = pos + 1;
        ScreenPage = SreenData[pos];
        WebSerial.println("Scrn Pg");
        return 1;
      }
      else if(SreenData[1] == 6){ //Btn Comand
        pos = pos + 1;
        ButtonPressed = SreenData[pos];
        WebSerial.println("Scrn BTN");
        return 1;
      }
      else if(SreenData[1] == 30){ //Spcl Button Command
        //Serial.read();
        //Serial.read();
        pos = pos + 3;
        ReflowMode = SreenData[pos];
        WebSerial.println("Scrn StartStop");
        return 1;
      }
      return 0;
    }
    else{
      if(pos < Lenth){
        pos++;
      }
    }
  }
  return 0;
}

unsigned char RequestPage(){
    char Data[20];
    unsigned char pos = 0;
    unsigned char pg = 0;
    Serial.write(0);
    Serial.write(10);
    Serial.write(0);
    Serial.write(10);
    while (1)
    {
      if(Serial.available() >= 6){
        unsigned char l = Serial.available();
        for(unsigned char k=0;k<l;k++){
          Data[k] = Serial.read();
          //WebSerial.print(SreenData[k]);
        }
        if(Data[pos] == 5){
          WebSerial.println("Scn Pct");
          pos = pos + 1;
          if(Data[pos] == 10){ //Fourm Comand
            pos = pos + 3;
            pg = Data[pos];
            WebSerial.println("Scrn Pg rcvd");
            break;
          }
        }
        else{
          if(pos < l){
            pos++;
          }
          else{
            break;
          }        
        }
      }
    }
  return pg;
}

char GetGuiWiFi(){
  return WiFiMode;
}

char GetGuiReflow(){
  return ReflowMode;
}