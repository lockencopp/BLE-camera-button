#ifndef BUTTON_HANDLING_H
#define BUTTON_HANDLING_H

#include <Arduino.h>

#define FOCUS_PIN 4   // Pin for focus action
#define SHUTTER_PIN 5 // Pin for shutter action

extern uint32_t button_time_now;  // Current time for button state
extern uint32_t button_time_last; // Last time for button state

extern uint8_t button_state_now;  // Current State of the button
extern uint8_t button_state_last; // Last State of the button

extern bool button_hold_flag;    // Flag to indicate if the button is held down
extern bool button_shutter_flag; // Flag to indicate if the button is pressed for shutter action
extern bool button_focus_flag;   // Flag to indicate if the button is pressed for focus action

void setup_outputs(); // Function to set up output pins
void handleButtonState();

#endif // BUTTON_HANDLING_H