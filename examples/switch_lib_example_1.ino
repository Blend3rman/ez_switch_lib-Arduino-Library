/*
   Ron D Bentley, Stafford, UK
   Mar 2021

   -      Example of use of the switch_lib library        -
   Reading single button switch to turn built in led on/off
   ''''''''''''''''''''''''''''''''''''''''''''''''''''''''

   This example and code is in the public domain and
   may be used without restriction and without warranty.

*/
#include <Arduino.h>
#include <switch_lib.h>  // switch_lib .h & .cpp files are stored under ...\Arduino\libraries\switch_lib\

int  switch_id;
bool led_level = LOW;

#define num_switches 1  // only a single switch in this sketch example

// Declare/define the switch instance of given size
Switches my_switches(num_switches);

void setup() {
  // attach a button switch to digital pin 3, no pull down resistor
  // and store the switch's id for later use.
  switch_id = my_switches.add_switch(button_switch, 3, circuit_C2);

  // validate the return
  if (switch_id < 0) {
    // error returned - there is a data compatibilty mismatch (-2),
    // or no room left to add switch (-1).
    Serial.begin(9600);
    Serial.println(F("Failure to add a switch."));
    if (switch_id == -1) {
      Serial.println(F("add_switch - no room to create given switch."));
    } else {
      // can only be that data for switch is invalid
      Serial.println(F("add_switch - one or more parameters is/are invalid."));
    }
    Serial.println(F("!! PROGRAMME TERMINATED !!"));
    Serial.flush();
    exit(1);
  }
  // inititialise built in led and turn to off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // keep reading the switch we have created and toggle the built in
  // led on/off for each press.
  do {
    if (my_switches.read_switch(switch_id) == switched) {
      led_level = HIGH - led_level;  // flip between HIGH and LOW each cycle
      digitalWrite(LED_BUILTIN, led_level);
    }
  } while (true);
}
