#include <Arduino.h>
#include "button_handling.h"

uint32_t button_time_now = 0;  // Current time for button state
uint32_t button_time_last = 0; // Last time for button state

uint8_t button_state_now = 0;  // Current State of the button
uint8_t button_state_last = 0; // Last State of the button

bool button_hold_flag = false;    // Flag to indicate if the button is held down
bool button_shutter_flag = false; // Flag to indicate if the button is pressed for shutter action
bool button_focus_flag = false;   // Flag to indicate if the button is pressed for focus action

// Function to handle button state changes

void set_focus_output()
{
    // Set the focus output pin to HIGH
    digitalWrite(FOCUS_PIN, HIGH);
}

void clear_focus_output()
{
    // Set the focus output pin to LOW
    digitalWrite(FOCUS_PIN, LOW);
}

void set_shutter_output()
{
    // Set the shutter output pin to HIGH
    digitalWrite(SHUTTER_PIN, HIGH);
}

void clear_shutter_output()
{
    // Set the shutter output pin to LOW
    digitalWrite(SHUTTER_PIN, LOW);
}

void setup_outputs()
{
    // Initialize the pins for focus and shutter actions
    pinMode(FOCUS_PIN, OUTPUT);
    pinMode(SHUTTER_PIN, OUTPUT);
    clear_focus_output();   // Set focus output to LOW initially
    clear_shutter_output(); // Set shutter output to LOW initially
}

// Helper function to handle focus action start
void handleFocusAction()
{
    button_focus_flag = true; // Set the focus flag
    Serial.println("Focus action start.");
    set_focus_output(); // Set the focus output pin to HIGH
}

// Helper function to handle shutter action start
void handleShutterAction()
{
    button_shutter_flag = true; // Set the shutter flag
    Serial.println("Shutter action start.");
    set_shutter_output(); // Set the shutter output pin to HIGH
}

// Helper function to handle hold action start
void handleHoldStart()
{
    button_hold_flag = true; // Set the hold flag
    Serial.println("Button hold action start.");
    set_shutter_output(); // Set the shutter output pin to HIGH
}

// Helper function to handle hold action stop
void handleHoldStop()
{
    button_hold_flag = false; // Reset the hold flag
    Serial.println("Button hold action stop.");
    clear_shutter_output(); // Set the shutter output pin to LOW
}

// Helper function to handle focus action stop
void handleFocusStop()
{
    button_focus_flag = false; // Reset the focus flag
    Serial.println("Focus action stop.");
    clear_focus_output(); // Set the focus output pin to LOW
}

// Helper function to handle shutter action stop
void handleShutterStop()
{
    button_shutter_flag = false; // Reset the shutter flag
    Serial.println("Shutter action stop.");
    clear_shutter_output(); // Set the shutter output pin to LOW
}

void handleButtonState()
{
    if (button_state_now != button_state_last)
    {
        // Button state has changed
        if (button_state_now == 1)
        {
            // Button pressed
            // button_time_last = millis(); // Update the last button press time
        }
        else if (button_state_now == 0)
        {
            // Button released
            uint32_t time_diff = millis() - button_time_last;

            if (time_diff < 50)
            {
                handleFocusAction(); // Handle focus action
            }
            else if (time_diff <= 350)
            {
                handleShutterAction(); // Handle shutter action
            }
            else
            {
                handleHoldStop(); // Handle hold stop action
            }
        }
        // Update the last button state
        button_state_last = button_state_now;
        button_time_last = millis(); // Update the last button press time
    }
    else if (button_state_now == 1)
    {
        // Button is still pressed
        uint32_t time_diff = millis() - button_time_last;
        if (time_diff > 350 && !button_hold_flag)
        {
            handleHoldStart(); // Handle hold start action
        }
    }
    else if (button_state_now == 0)
    {
        // Button is still released
        uint32_t time_diff = millis() - button_time_last;
        if (time_diff > 50)
        {
            if (button_focus_flag)
            {
                handleFocusStop(); // Handle focus stop action
            }
            else if (button_shutter_flag)
            {
                handleShutterStop(); // Handle shutter stop action
            }
        }
    }
}