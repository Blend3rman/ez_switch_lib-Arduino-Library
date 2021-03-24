// Arduino Switch Library for configuring different switch type wired
// in common circuit schemes.
//
// Ron Bentley, Stafford (UK), March 2021, version 1.00
//
// This example and code is in the public domain and
// may be used without restriction and without warranty.
//

#include <Arduino.h>
#include <ez_switch_lib.h>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Set up switch cotrol structure and initialise internal variables.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Switches::Switches(byte max_switches)
{
  // Establish the switch control structure (switches) of the size required
  switches = malloc(sizeof(*switches) * max_switches);
  if (switches == NULL) {
    // malloc failure
    Serial.begin(9600);
    Serial.println("!!Failure to acquire memory of required size - PROGRAM TERMINATED!!");
    Serial.flush();
    exit(1);
  }

  // Initialise private variables
  _num_entries  = 0;            // will be incremented each time a switch is added, up to _max_switches
  _max_switches = max_switches; // transfer to internal variable
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Generic switch read function.
// Read the switch defined by the function parameter.
// Function returns a value indicating if the switch
// has undergone a transition or not.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool Switches::read_switch(byte sw) {
  bool sw_status;
  if (sw < 0 || sw >= _num_entries) return !switched; // out of range, slot 'sw' is not configured with a switch
  if (switches[sw].switch_type == button_switch) {
    sw_status = read_button_switch(sw);
  } else {
    sw_status = read_toggle_switch(sw);
  }
  // now determine if switch has output pin associated and if switched
  // flip the output's status, ie HIGH->LOW, or LOW->HIGH
  if (sw_status == switched && switches[sw].switch_out_pin > 0)
   {
     // flip the output level of associated switch output pin, if defined
     bool status = switches[sw].switch_out_pin_status; // last status value of out_pin
     status = HIGH - status;                           // flip the status value
     switches[sw].switch_out_pin_status = status;      // update current status value
     digitalWrite(switches[sw].switch_out_pin, status);
   }
   return sw_status;
}  // End of read_switch

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Generic toggle switch read function.
// Test the toggle switch to see if its status has changed since last look.
// Note that although switch status is a returned value from the function,
// the current status of the switch ('switches[sw].switch_status') is always
// maintained and can be tested outside of the function at any point/time.
// It will either have a status of 'on' or '!on' (ie off).
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool Switches::read_toggle_switch(byte sw) {
  byte switch_pin_reading = digitalRead(switches[sw].switch_pin);  // test current state of toggle pin
  if (switches[sw].switch_circuit_type == circuit_C2) {
    // Need to invert HIGH/LOW if circuit design sets
    // pin HIGH representing switch in off state.
    // ie inititialised as INPUT_PULLUP
    switch_pin_reading = !switch_pin_reading;
  }
  if (switch_pin_reading != switches[sw].switch_status && !switches[sw].switch_pending) {
    // Switch change detected so start debounce cycle
    switches[sw].switch_pending = true;
    switches[sw].switch_db_start = millis();  // set start of debounce timing
  }
    if (switches[sw].switch_pending) {
      // We are in the switch transition cycle so check if debounce period has elapsed
      if (millis() - switches[sw].switch_db_start >= _debounce) {
        // Debounce period elapsed so assume switch has settled down after transition
        switches[sw].switch_status  = !switches[sw].switch_status;  // flip status
        switches[sw].switch_pending = false;                        // cease transition cycle
        return switched;
     }
  }
  return !switched;
} // End of read_toggle_switch

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Read button switch function.
// Generic button switch read function.
// Reading is controlled by:
//   a. the function parameter which indicates which switch
//      is to be polled, and
//   b. the switch control struct(ure), referenced by a).
//
// Note that this function works in a nonexclusive way
// and incorporates debounce code.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool Switches::read_button_switch(byte sw) {
  byte switch_pin_reading = digitalRead(switches[sw].switch_pin);
  if (switch_pin_reading == switches[sw].switch_on_value) {
    // Switch is pressed (ON), so start/restart debounce process
    switches[sw].switch_pending = true;
    switches[sw].switch_db_start   = millis();  // start elapse timing
    return !switched;                           // now waiting for debounce to conclude
  }
  if (switches[sw].switch_pending && switch_pin_reading != switches[sw].switch_on_value) {
    // Switch was pressed, now released (OFF), so check if debounce time elapsed
    if (millis() - switches[sw].switch_db_start >= _debounce) {
      // dounce time elapsed, so switch press cycle complete
      switches[sw].switch_pending = false;
      return switched;
    }
  }
  return !switched;
}  // End of read_button_switch

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Add given switch to switch control structure, but validate
// given paramters and ensure there is a free slot.
//
// Return values from add_switch are:
//
//    >= 0 the switch control structure entry number ('switch_id')
//         for the switch added,
//      -1 add_falure - no slots available in the switch
//         control structure,
//      -2 bad_params - given paramter(s) for switch are
//         not valid.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int Switches::add_switch(byte sw_type, byte sw_pin, byte circ_type) {
  if ((sw_type != button_switch && sw_type != toggle_switch) ||
      (circ_type != circuit_C1 && circ_type != circuit_C2)) return bad_params;  // bad paramters
  if (_num_entries < _max_switches) {
    // room to add another switch...initialise the switch's
    // data depending on type of switch and circuit
    switches[_num_entries].switch_type    = sw_type;
    switches[_num_entries].switch_pin     = sw_pin;
    switches[_num_entries].switch_circuit_type   = circ_type;
    switches[_num_entries].switch_pending = false;
    switches[_num_entries].switch_db_start   = 0;
    if (circ_type == circuit_C1) {
      switches[_num_entries].switch_on_value = HIGH;
    } else {
      switches[_num_entries].switch_on_value = LOW;
    }
    if (sw_type == button_switch) {
      switches[_num_entries].switch_status = not_used;
    } else {
      switches[_num_entries].switch_status = !on;
    }
    pinMode(sw_pin, circ_type);  // establish pin set up
    // ensure no mapping to an output pin until created explicitly
    switches[_num_entries].switch_out_pin        = 0;
    switches[_num_entries].switch_out_pin_status = LOW;  // set LOW unless explicitly changed

    _num_entries++;              // point to next free slot
    return _num_entries - 1;     // return 'switch_id' - given switch now added to switch control structure
  } else return add_failure;     // no room left to add another switch!
}  // End add_switch


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Link or delink the given switch to the given digital pin as an output
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int Switches::link_switch_to_output(byte switch_id, byte output_pin, bool HorL){
  if(switch_id > _num_entries) {return link_failure;} // no such switch
  if (output_pin == 0){
    // delink this output from this switch, set te output to the required level first
    if (switches[switch_id].switch_out_pin == 0){
      // no output pin previously defined
      return link_failure;
    }
    // set existing pin to level required state before clearing the link
    digitalWrite(switches[switch_id].switch_out_pin, HorL); 
  }else {
    // initialise given output pin
    pinMode(output_pin, OUTPUT); 
    digitalWrite(output_pin, HorL); // set to param value until switched
  }
  switches[switch_id].switch_out_pin        = output_pin;
  switches[switch_id].switch_out_pin_status = HorL;
  return link_success;           // success
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Return the number of slots left unused
// in the switch control structure.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int Switches::num_free_switch_slots() {
  return _max_switches - _num_entries;
} // End num_free_switch_slots

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Set debounce period (milliseconds).
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Switches::set_debounce(int period) {
  if (period >= 0)  _debounce = period;
}  // End set_debounce

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Print given switch control data.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Switches::print_switch(byte sw) {
  if (0 <= sw && sw < _num_entries ) {
    Serial.print("slot: ");
    Serial.print(sw);
    Serial.print("  sw_type= ");
    Serial.print(switches[sw].switch_type);
    Serial.print("\tsw_pin= ");
    Serial.print(switches[sw].switch_pin);
    Serial.print("\tcirc_type= ");
    Serial.print(switches[sw].switch_circuit_type);
    Serial.print("\tpending= ");
    Serial.print(switches[sw].switch_pending);
    Serial.print("\tdb_start= "); 
    Serial.print(switches[sw].switch_db_start);
    Serial.print("\ton_value= ");
    Serial.print( switches[sw].switch_on_value);
    Serial.print("\tsw_status= ");
    Serial.println(switches[sw].switch_status);
    Serial.print("\t\t\top_pin= ");
    Serial.print(switches[sw].switch_out_pin);
    Serial.print("\top_status= ");
    Serial.println(switches[sw].switch_out_pin_status);
    Serial.flush();
  }
} // End print_switch

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Print all switch control data set up.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Switches::print_switches() {
  Serial.println(F("\nDeclared & configured switches:"));
  for (byte sw = 0; sw < _num_entries; sw++) {
    print_switch(sw);
  }
} // End print_switches
