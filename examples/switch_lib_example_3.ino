/*
   Ron D Bentley, Stafford, UK
   Mar 2021

   -      Example of use of the switch_lib library        -
       Reading button & toggle switches to turn on/off LEDs
   ''''''''''''''''''''''''''''''''''''''''''''''''''''''''

   This example and code is in the public domain and
   may be used without restriction and without warranty.

*/
#include <Arduino.h>
#include <switch_lib.h>  // switch_lib .h & .cpp files are stored under ...\Arduino\libraries\switch_lib\

int  switch_id;

#define not_configured     255  // used to indicate if a switch_control data entry has be configured

#define num_switches 4

// we will use a struct(ure) data type to keep our switch/LED
// data tidy and readily accessible
struct switch_control {
  byte  sw_type;            // type of switch connected
  byte  sw_pin;             // digital input pin assigned to the switch
  byte  sw_circuit_type;    // the type of circuit wired to the switch
  byte   sw_id;              // holds the switch id given by the add.switch function for this switch
  byte  sw_led_pin;         // digital pin connecting the LED for this switch
  bool  sw_led_status;      // current status LOW/HIGH of the LED connected to this switch
} btl[num_switches] = {     // 'btl' = buttons, toggles & LEDs

  button_switch, 6, circuit_C1, not_configured, 2, LOW, // start with LED status LOW
  button_switch, 7, circuit_C2, not_configured, 3, LOW,
  toggle_switch, 8, circuit_C1, not_configured, 4, LOW,
  toggle_switch, 9, circuit_C2, not_configured, 5, LOW
};


// Declare/define the switch instance of given size
Switches my_switches(num_switches);

void setup() {
  // attach each switches to its defined digital pin/circuit type
  // and store the switch's id back in its struct entry for later use.
  for (byte sw = 0; sw < num_switches; sw++) {
    switch_id  = my_switches.add_switch(btl[sw].sw_type,
                                        btl[sw].sw_pin,
                                        btl[sw].sw_circuit_type);
    // validate the return
    if (switch_id < 0) {
      // error returned - there is a data compatibility  mismatch (-2),
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
    btl[sw].sw_id = switch_id; // store given switch id for this sw for use later
    // now initialise the switch's associated LED and turn off
    pinMode(btl[sw].sw_led_pin, OUTPUT);
    digitalWrite(btl[sw].sw_led_pin, btl[sw].sw_led_status);
  }
}

void loop() {
  // keep reading the switches we have created and toggle their
  // associated LEDs on/off
  do {
    for (byte sw = 0; sw < num_switches; sw++) {
      if (my_switches.read_switch(btl[sw].sw_id) == switched) {
        btl[sw].sw_led_status = HIGH - btl[sw].sw_led_status;  // flip between HIGH and LOW each cycle
        digitalWrite(btl[sw].sw_led_pin, btl[sw].sw_led_status);
      }
    }
  } while (true);
}
