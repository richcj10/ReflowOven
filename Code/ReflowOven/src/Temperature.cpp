#include "Arduino.h"
#include <Wire.h>
#include <SparkFun_MCP9600.h>
#include "Temperature.h"
#include <WebSerial.h>

#define BOX_CAR_SIZE 3

int TempMode = 0;
float TempBoxCar[BOX_CAR_SIZE+1];
MCP9600 tempSensor;
uint8_t risingAlert = 1; //What alert to use for detecting cold -> hot transitions.
uint8_t fallingAlert = 3; //What alert to use for detecting hot -> cold transitions.
                          //These numbers are arbitrary and can be anything from 1 to 4, but just can't be equal!

float alertTemp = 29.5;  //What temperature to trigger the alert at (before hysteresis).
                        //This is about the surface temperature of my finger, but please change this if 
                        //you have colder/warmer hands or if the ambient temperature is different.
uint8_t hysteresis = 2; //How much hysteresis to have, in degrees Celcius. Feel free to adjust this, but 2°C seems to be about right.
void TempFilterSetup();
int TempSetup(){
    Wire.begin(2,4);
    Wire.setClock(100000);
    tempSensor.begin();       // Uses the default address (0x60) for SparkFun Thermocouple Amplifier
    //tempSensor.begin(0x66); // Default address (0x66) for SparkX Thermocouple Amplifier

  //check if the sensor is connected
  if(tempSensor.isConnected()){
    //Serial.println("Device will acknowledge!");
    TempMode = 1;
  }
  else {
    Serial.println("Bad TC Device: Freezing.");
    while(1); //hang forever
    TempMode = -1;
    return 0;
  }

  //check if the Device ID is correct
  if(tempSensor.checkDeviceID()){
    //Serial.println("Device ID is correct!");     
    TempMode = 2;  
    TempFilterSetup(); 
  }
  else {
    Serial.println("Device ID Bad! Freezing.");
    while(1); //hang forever
    TempMode = -2;
    return 0;
  }
  return 1;
}

int randomNumber = 0;
int i = 0;
float TempTotal = 0;
float ReturnTemp = 0;

void TempFilterSetup(){
  for(i = 0;i<BOX_CAR_SIZE;i++){
    TempBoxCar[i] = tempSensor.getThermocoupleTemp();
    delay(100);
  }
}

float TempRead(){
  if(TempMode > 0){
    TempTotal = 0;
    for(i = 0;i<BOX_CAR_SIZE;i++){
      TempBoxCar[i] = TempBoxCar[i+1];
      //Serial.print("I = ");
      //Serial.println(i);
      //Serial.print("Prev Temp = ");
      //Serial.println(TempBoxCar[i]);
      TempTotal = TempTotal + TempBoxCar[i];
    }
    //Serial.print("Total Temp = ");
    //Serial.println(TempTotal);
    TempBoxCar[BOX_CAR_SIZE] = tempSensor.getThermocoupleTemp();
    TempTotal = TempTotal + TempBoxCar[BOX_CAR_SIZE];
    //Serial.print("Total Temp = ");
    //Serial.println(TempTotal);
    ReturnTemp = (TempTotal/BOX_CAR_SIZE+1);
    //Serial.print(" Temp = ");
    //Serial.println(ReturnTemp);
    //Serial.print(" °C   Ambient: ");
    //Serial.println(tempSensor.getAmbientTemp());
    //Serial.print(" °C   Temperature Delta: ");
    //Serial.print(tempSensor.getTempDelta());
    //Serial.print(" °C");

    //Serial.println(); 
    return ReturnTemp;
  }
  else{
    randomNumber = randomNumber + 1;
    return randomNumber;
  }
}

float GetOvenTemp(){
  return ReturnTemp;
}