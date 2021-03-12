/*
   Ron D Bentley, Stafford, UK
   Mar 2021
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   -         Example of use of the switch_lib library          -
   Example 1.2
   Reading single button switch to turn built in led on/off, but
   in this example we shall link the switch to an output pin
   (LED_BUILTIN) using a switch_lib function, so that when
   actuated, the output pin will be automatically flipped
   HIGH-LOW etc each time the button switch is pressed WITHOUT
   any further coding.
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

   This example and code is in the public domain and
   may be used without restriction and without warranty.

*/
#include <Arduino.h>
#include <switch_lib.h>  // switch_lib .h & .cpp files are stored under ...\Arduino\libraries\switch_lib\

int  switch_id;

#define num_switches 1  // only a single switch in this sketch example

// Declare/define the switch instance of given size
Switches my_switches(num_switches);

void setup() {
  // Attach a button switch to digital pin 2, with
  // an external pull down resistor, circuit_C1,
  // and store the switch's id for later use.
  switch_id = my_switches.add_switch(
                button_switch,
                2,
                circuit_C1);
  // validate the return
  if (switch_id < 0) {
    // Error returned - there is a data compatibilty mismatch (-2, bad_params),
    // or no room left to add switch (-1, add_failure).
    Serial.begin(9600);
    Serial.println(F("Failure to add a switch"));
    if (switch_id == add_failure) {
      Serial.println(F("add_switch - no room to create given switch"));
    } else {
      // can only be that data for switch is invalid
      Serial.println(F("add_switch - one or more parameters is/are invalid"));
    }
    Serial.println(F("!PROGRAM TERMINATED!!"));
    Serial.flush();
    exit(1);
  }

  // Link/associate this switch to the in built led (normally pin 13)
  // with the switch we have justed installed/created so that every
  // time the switch is actuated the built in LED will be automatically
  // flipped.  Start with LED at low setting.
  int link_result = my_switches.link_switch_to_output(
                      switch_id,
                      LED_BUILTIN,
                      LOW);
  if (link_result == link_failure ) {
    // linking failed, invalid switch id
    Serial.begin(9600);
    Serial.println(F("Failure to link an output to a switch"));
    Serial.println(F("!!PROGRAM TERMINATED!!"));
    Serial.flush();
    exit(2);
  }
}

void loop() {
  do {
    // just keep reading, LED_BUILTIN will automatically be flipped for us
    // so we dont need to do anything else
    my_switches.read_switch(switch_id);
  }
  while (true);
}
