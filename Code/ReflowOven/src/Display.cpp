#include <genieArduino.h>
#include "Display.h"

Genie genie;
#define RESETLINE 4  // Change this if you are not using an Arduino Adaptor Shield Version 2 (see code below)
void DisplaySetup(){
  genie.Begin(Serial);   // Use Serial0 for talking to the Genie Library, and to the 4D Systems display

  genie.AttachEventHandler(myGenieEventHandler); // Attach the user function Event Handler for processing events

  pinMode(RESETLINE, OUTPUT);  // Set D4 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  digitalWrite(RESETLINE, 1);  // Reset the Display via D4
  delay(100);
  digitalWrite(RESETLINE, 0);  // unReset the Display via D4

  // Let the display start up after the reset (This is important)
  // Increase to 4500 or 5000 if you have sync problems as your project gets larger. Can depent on microSD init speed.
  //delay (3500); 

  //genie.WriteContrast(10); // About 2/3 Max Brightness

  //genie.WriteStr(0, GENIE_VERSION);
}

void DisplayUpdate()
{
  genie.DoEvents(); // This calls the library each loop to process the queued responses from the display

  // Write to CoolGauge0 with the value in the gaugeVal variable
  genie.WriteObject(GENIE_OBJ_COOL_GAUGE, 0, 50);

  //gaugeVal += gaugeAddVal;

  // The results of this call will be available to myGenieEventHandler() after the display has responded
  genie.ReadObject(GENIE_OBJ_USER_LED, 0); // Do a manual read from the UserLEd0 object
}

void myGenieEventHandler(void)
{
  genieFrame Event;
  genie.DequeueEvent(&Event);

  int slider_val = 0;

  //Filter Events from Slider0 (Index = 0) for a Reported Message from Display
  if (genie.EventIs(&Event, GENIE_REPORT_EVENT, GENIE_OBJ_SLIDER, 0))
  {
    slider_val = genie.GetEventData(&Event);  // Receive the event data from the Slider0
    genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, slider_val);     // Write Slider0 value to LED Digits 0
  }

  //Filter Events from UserLed0 (Index = 0) for a Reported Object from Display (triggered from genie.ReadObject in User Code)
  if (genie.EventIs(&Event, GENIE_REPORT_OBJ,   GENIE_OBJ_USER_LED, 0))
  {
    bool UserLed0_val = genie.GetEventData(&Event);               // Receive the event data from the UserLed0
    UserLed0_val = !UserLed0_val;                                 // Toggle the state of the User LED Variable
    genie.WriteObject(GENIE_OBJ_USER_LED, 0, UserLed0_val);       // Write UserLed0_val value back to UserLed0
  }
}