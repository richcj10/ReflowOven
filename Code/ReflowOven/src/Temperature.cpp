#include "Arduino.h"
#include <Wire.h>
#include <SparkFun_MCP9600.h>
#include "Temperature.h"

int TempMode = 0;
MCP9600 tempSensor;
uint8_t risingAlert = 1; //What alert to use for detecting cold -> hot transitions.
uint8_t fallingAlert = 3; //What alert to use for detecting hot -> cold transitions.
                          //These numbers are arbitrary and can be anything from 1 to 4, but just can't be equal!

float alertTemp = 29.5;  //What temperature to trigger the alert at (before hysteresis).
                        //This is about the surface temperature of my finger, but please change this if 
                        //you have colder/warmer hands or if the ambient temperature is different.
uint8_t hysteresis = 2; //How much hysteresis to have, in degrees Celcius. Feel free to adjust this, but 2°C seems to be about right.

int TempSetup(){
    Wire.begin(2,4);
    Wire.setClock(100000);
    tempSensor.begin();       // Uses the default address (0x60) for SparkFun Thermocouple Amplifier
    //tempSensor.begin(0x66); // Default address (0x66) for SparkX Thermocouple Amplifier

  //check if the sensor is connected
  if(tempSensor.isConnected()){
    Serial.println("Device will acknowledge!");
    TempMode = 1;
  }
  else {
    //Serial.println("Device did not acknowledge! Freezing.");
    //while(1); //hang forever
    TempMode = -1;
    return 0;
  }

  //check if the Device ID is correct
  if(tempSensor.checkDeviceID()){
    Serial.println("Device ID is correct!");     
    TempMode = 2;   
  }
  else {
    //Serial.println("Device ID is not correct! Freezing.");
    //while(1); //hang forever
    TempMode = -2;
    return 0;
  }
  return 1;
}

int randomNumber = 0;

float TempRead(){
  if(TempMode > 0){
    //Serial.print("Thermocouple: ");
    //Serial.println(tempSensor.getThermocoupleTemp());
    //Serial.print(" °C   Ambient: ");
    //Serial.println(tempSensor.getAmbientTemp());
    //Serial.print(" °C   Temperature Delta: ");
    //Serial.print(tempSensor.getTempDelta());
    //Serial.print(" °C");

    //Serial.println(); 
    return tempSensor.getThermocoupleTemp();
  }
  else{
    randomNumber = randomNumber + 1;
    return randomNumber;
  }
}