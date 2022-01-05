#include <genieArduino.h>
#include "Display.h"

Genie genie;
#define RESETLINE 4  // Change this if you are not using an Arduino Adaptor Shield Version 2 (see code below)
void DisplaySetup()
{
  Serial.begin(115200);  // Serial0 @ 200000 (200K) Baud
  genie.Begin(Serial);   // Use Serial0 for talking to the Genie Library, and to the 4D Systems display

  genie.AttachEventHandler(myGenieEventHandler); // Attach the user function Event Handler for processing events

  pinMode(RESETLINE, OUTPUT);  // Set D4 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  digitalWrite(RESETLINE, 1);  // Reset the Display via D4
  delay(100);
  digitalWrite(RESETLINE, 0);  // unReset the Display via D4

  // Let the display start up after the reset (This is important)
  // Increase to 4500 or 5000 if you have sync problems as your project gets larger. Can depent on microSD init speed.
  delay (3500); 

  genie.WriteContrast(10); // About 2/3 Max Brightness

  genie.WriteStr(0, GENIE_VERSION);
}

void DislayLoop()
{
  static long waitPeriod = millis();
  static int gaugeAddVal = 1; // Simulation code variable. Value to change the gauge by each loop
  static int gaugeVal = 50; // Simulation code variable. Value to start the gauge at when powered on

  genie.DoEvents(); // This calls the library each loop to process the queued responses from the display

  if (millis() >= waitPeriod)
  {
    // Write to CoolGauge0 with the value in the gaugeVal variable
    genie.WriteObject(GENIE_OBJ_COOL_GAUGE, 0, gaugeVal);

    // Simulation code, just to increment and decrement gauge value each loop, for animation
    gaugeVal += gaugeAddVal;
    if (gaugeVal == 99) gaugeAddVal = -1;
    if (gaugeVal == 0) gaugeAddVal = 1;

    // The results of this call will be available to myGenieEventHandler() after the display has responded
    genie.ReadObject(GENIE_OBJ_USER_LED, 0); // Do a manual read from the UserLEd0 object

    waitPeriod = millis() + 50; // rerun this code to update Cool Gauge and Slider in another 50ms time.
  }
}

/////////////////////////////////////////////////////////////////////
//
// This is the user's event handler. It is called by genieDoEvents()
// when the following conditions are true
//
//		The link is in an IDLE state, and
//		There is an event to handle
//
// The event can be either a REPORT_EVENT frame sent asynchronously
// from the display or a REPORT_OBJ frame sent by the display in
// response to a READ_OBJ (genie.ReadObject) request.
//

/* COMPACT VERSION HERE, LONGHAND VERSION BELOW WHICH MAY MAKE MORE SENSE
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
} */

// LONG HAND VERSION, MAYBE MORE VISIBLE AND MORE LIKE VERSION 1 OF THE LIBRARY
void myGenieEventHandler(void)
{
  genieFrame Event;
  genie.DequeueEvent(&Event); // Remove the next queued event from the buffer, and process it below

  int slider_val = 0;

  //If the cmd received is from a Reported Event (Events triggered from the Events tab of Workshop4 objects)
  if (Event.reportObject.cmd == GENIE_REPORT_EVENT)
  {
    if (Event.reportObject.object == GENIE_OBJ_SLIDER)                // If the Reported Message was from a Slider
    {
      if (Event.reportObject.index == 0)                              // If Slider0 (Index = 0)
      {
        slider_val = genie.GetEventData(&Event);                      // Receive the event data from the Slider0
        genie.WriteObject(GENIE_OBJ_LED_DIGITS, 0, slider_val);       // Write Slider0 value to LED Digits 0
      }
    }
  }

  //If the cmd received is from a Reported Object, which occurs if a Read Object (genie.ReadOject) is requested in the main code, reply processed here.
  if (Event.reportObject.cmd == GENIE_REPORT_OBJ)
  {
    if (Event.reportObject.object == GENIE_OBJ_USER_LED)              // If the Reported Message was from a User LED
    {
      if (Event.reportObject.index == 0)                              // If UserLed0 (Index = 0)
      {
        bool UserLed0_val = genie.GetEventData(&Event);               // Receive the event data from the UserLed0
        UserLed0_val = !UserLed0_val;                                 // Toggle the state of the User LED Variable
        genie.WriteObject(GENIE_OBJ_USER_LED, 0, UserLed0_val);       // Write UserLed0_val value back to UserLed0
      }
    }
  }

  /********** This can be expanded as more objects are added that need to be captured *************
  *************************************************************************************************
  Event.reportObject.cmd is used to determine the command of that event, such as an reported event
  Event.reportObject.object is used to determine the object type, such as a Slider
  Event.reportObject.index is used to determine the index of the object, such as Slider0
  genie.GetEventData(&Event) us used to save the data from the Event, into a variable.
  *************************************************************************************************/
}