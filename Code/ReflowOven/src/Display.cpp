#include <genieArduino.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Display.h"
#include "Temperature.h"
#include "Version.h"

Genie genie;

char WiFiMode = 0;
#define RESETLINE 4  // Change this if you are not using an Arduino Adaptor Shield Version 2 (see code below)
void DisplaySetup(){
  genie.Begin(Serial);   // Use Serial0 for talking to the Genie Library, and to the 4D Systems display

  genie.AttachEventHandler(myGenieEventHandler); // Attach the user function Event Handler for processing events

  //pinMode(RESETLINE, OUTPUT);  // Set D4 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  //digitalWrite(RESETLINE, 1);  // Reset the Display via D4
  //delay(100);
  //digitalWrite(RESETLINE, 0);  // unReset the Display via D4

  // Let the display start up after the reset (This is important)
  // Increase to 4500 or 5000 if you have sync problems as your project gets larger. Can depent on microSD init speed.
  //delay (3500); 

  //genie.WriteContrast(10); // About 2/3 Max Brightness

  //genie.WriteStr(0, GENIE_VERSION);
}
bool LED = 0;
char FirstTime = 0;
void DoOnce(){
  genie.WriteStr(3, String(VERSION));
  genie.WriteStr(4, "0:0:0:0");
}

void DisplayUpdate()
{
  int TempValue = int(TempRead());
  //Serial.println("Hi");
   // This calls the library each loop to process the queued responses from the display
  if(FirstTime == 0){
    FirstTime = 1;
    DoOnce();
  }
  // Write to CoolGauge0 with the value in the gaugeVal variable
  if(WiFiMode == 1){
    genie.WriteObject(GENIE_OBJ_USER_LED, 0, 1);
  }
  genie.WriteObject(GENIE_OBJ_SCOPE, 0, TempValue);
  genie.WriteStr(0, String(TempValue));
}

void myGenieEventHandler(void)
{
  genieFrame Event;
  genie.DequeueEvent(&Event);
}

void SetWifiConnect(char mode){
  WiFiMode = mode;
}

void DisplayRead(){
  genie.DoEvents();
}