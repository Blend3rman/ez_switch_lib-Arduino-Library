/*
   Ron D Bentley, Stafford, UK
   Feb 2021

   -    Example of use of the switch_lib library     -
   Reading Multiple Switch Types, using simple polling
   '''''''''''''''''''''''''''''''''''''''''''''''''''

   This example and code is in the public domain and
   may be used without restriction and without warranty.

*/
#include <Arduino.h>
#include <switch_lib.h>  // switch_lib .h & .cpp files are stored under ...\Arduino\libraries\switch_lib\

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Declare/define specific 'my_data' for 'my_project'
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define num_switches 6

// Switch to Pin Macro Definition List:
#define my_button_pin_1      2  // digital pin number
#define my_button_pin_2      3  // etc
#define my_button_pin_3      4

#define my_toggle_pin_1      5
#define my_toggle_pin_2      6
#define my_toggle_pin_3      7

#define not_configured     255  // used to indicate if the my_switch_data entry has be configured

// Establish type of switch, assigned digital pin and circuit type
// for each switch we are connecting. Until we present each
// switch entry to the add.switch function it will not be
// recorded as configured, hence the use of the final column.
// We start with all defined switches as being 'not_configured'
// Note that:
// 'button_switch', 'toggle_switch', 'circuit_C1' and 'circuit_C2'
// are library defined macros.

byte my_switch_data[num_switches][4] =
{
  button_switch,  my_button_pin_1, circuit_C1, not_configured,
  button_switch,  my_button_pin_2, circuit_C2, not_configured,
  button_switch,  my_button_pin_3, circuit_C1, not_configured,
  toggle_switch,  my_toggle_pin_1, circuit_C2, not_configured,
  toggle_switch,  my_toggle_pin_2, circuit_C1, not_configured,
  toggle_switch,  my_toggle_pin_3, circuit_C2, not_configured
};

// Declare/define the switch instance of given size
Switches my_switches(num_switches);

//
// Set up connected switches as per 'my_switch_data' configs
//

void setup()
{
  Serial.begin(9600);
  // Create/install the defined switches...
  create_my_switches();
  my_switches.set_debounce(20);  // set debounce for 20 msecs
  // Report on my_data set up
  Serial.println(F("\nDeclared & configured switches:"));
  my_switches.print_switches();
  Serial.print(F("\nNumber of free switch slots = "));
  Serial.println(my_switches.num_free_switch_slots());
  Serial.flush();
}

void loop()
{
  do {
    // Poll all switches - examine each connected switch in turn and, if switched,
    // process its associated purpose.
    for (int switch_id = 0; switch_id < num_switches; switch_id++) {
      if (my_switches.read_switch(switch_id) == switched) {
        // This switch ('sw') has been pressed, so process via its switch-case code
        if (my_switches.switches[switch_id].switch_type == button_switch) {
          Serial.print(F("\nbutton switch on digital pin "));
        } else {
          Serial.print(F("\ntoggle switch on digital pin "));
        }
        byte my_switch_pin = my_switches.switches[switch_id].switch_pin;
        Serial.print(my_switch_pin);
        Serial.println(F(" triggered"));
        // Move to switch's associated code section
        switch (my_switch_pin)
        {
          case my_button_pin_1:
            // button switch 1 used to reveal the status of toggle switches
            // as their status is maintained every time they are actuated.
            Serial.println(F("case statement 1 entered"));
            print_toggle_status();
            Serial.flush();
            break;
          case my_button_pin_2:
            Serial.println(F("case statement 2 entered"));
            break;
          case my_button_pin_3:
            Serial.println(F("case statement 3 entered"));
            break;
          case my_toggle_pin_1:
            Serial.print(F("case statement 4 entered, switch is "));
            Serial.println(my_switches.switches[switch_id].switch_status);
            break;
          case my_toggle_pin_2:
            Serial.print(F("case statement 5 entered, switch is "));
            Serial.println(my_switches.switches[switch_id].switch_status);
            break;
          case my_toggle_pin_3:
            Serial.print(F("case statement 6 entered, switch is "));
            Serial.println(my_switches.switches[switch_id].switch_status);
            break;
          default:
            // Spurious switch index!  Should never arise as this is controlled
            // by the for loop within defined upper bound
            break;
        }
        Serial.flush();  // flush out the output buffer
      }
    }
  }
  while (true);
}

//
// Print the current status/setting of each toggle switch configured.
// We scan down my_switch_data to pick out toggle switches and if they
// configured access theit status.
//

void print_toggle_status() {
  Serial.println(F("toggle switches setting: "));
  for (byte sw = 0; sw < num_switches; sw++) {
    if (my_switch_data[sw][0] == toggle_switch &&
        my_switch_data[sw][3] != not_configured) {
      Serial.print(F("toggle switch on digital pin "));
      Serial.print(my_switch_data[sw][1]);
      Serial.print(F(" is "));
      byte switch_id = my_switch_data[sw][3]; // this is the position in the switch control struct for this switch
      if (my_switches.switches[switch_id].switch_status == on) {
        Serial.println(F("ON"));
      } else {
        Serial.println(F("OFF"));
      }
    }
  }
}

//
// Create a switch entry for each wired up switch, in accordance
// with 'my' declared switch data.
// add_switch params are - switch_type, digital pin number and circuit type.
// Return values from add_switch are:
//    >= 0 the switch control structure entry number ('switch_id') for the switch added,
//      -1 no slots available in the switch control structure,
//      -2 given paramter(s) for switch are not valid.

void create_my_switches() {
  for (int sw = 0; sw < num_switches; sw++) {
    int switch_id =
      my_switches.add_switch(my_switch_data[sw][0], // switch type
                             my_switch_data[sw][1], // digital pin number
                             my_switch_data[sw][2]);// circuit type
    if (switch_id < 0)
    { // There is a data compatibilty mismatch (-2),
      // or no room left to add switch (-1).
      Serial.print(F("Failure to add a switch:\nswitch entry:"));
      Serial.print(sw);
      Serial.print(F(", data line = "));
      Serial.print(my_switch_data[sw][0]);
      Serial.print(F(", "));
      Serial.print(my_switch_data[sw][1]);
      Serial.print(F(", "));
      Serial.println(my_switch_data[sw][2]);
      Serial.println(F("!! PROGRAMME TERMINATED !!"));
      Serial.flush();
      exit(1);
    } else {
      // 'switch_id' is the switch control slot entry for this switch (sw),
      // so we can use this, if required, to know where our switches are
      // in the control structure by keeping a note of them against their
      // my_switch_data config settings.
      my_switch_data[sw][3] = switch_id;
    }
  }
}  // End create_my_switches
