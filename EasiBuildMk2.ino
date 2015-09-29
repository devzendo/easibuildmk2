// EasiBuild Mk 2 Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//
// For Arduino Micro. If you want to target a different board, search for the comments 'CHANGE_BOARD';
// if you want to change the pins used for the various inputs/outputs, search for the comments 'CHANGE_PIN'.
//
//
// Acknowledgements:
// SCoop: Fabrice Oudert http://forum.arduino.cc/index.php?topic=137801.0

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// CHANGE_BOARD
#include <Arduino.h> 

#include <SCoop.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "PinAssignments.h"
#include "SidetoneOscillator.h"
#include "Display.h"

//==============================================================================
//=== PIN CHANGE INTERRUPT
//==============================================================================

/*
 *  value = portb & interesting_bits;
 *  if (value == last_portb) {
 *    return;
 *  }
 *
 *  last_portb = value;
 *  this_dah = value & dah;
 *  if (this_dah != last_dah) {
 *    last_dah = this_dah;
 *    raise(dah_event, this_dah ? HIGH : LOW);
 *  }
 *  this_dit = value & dit;
 *  if (this_dit != last_dit) {
 *    last_dit = this_dit;
 *    raise(dit_event, this_dit ? HIGH : LOW);
 *  }
 *  this_btn = value & btn;
 *  if (this_btn != last_btn) {
 *    last_btn = this_btn;
 *    raise(btn_event, this_btn ? HIGH : LOW);
 *  }
 *
 *  // do rotary encoder
 *
 */

//==============================================================================
//=== SCOOP TASKS
//==============================================================================

// user definition of task1 object
defineTask(Task1)
// force any read/write to memory
volatile long count;

void Task1::setup() {
    count = 0;
};

void Task1::loop() {
    sleepSync(1000);
    count++;
};


// user quick definition of task2 object 
defineTaskLoop(Task2)
{   digitalWrite(13, HIGH);
    sleep(100);
    digitalWrite(13, LOW);
    sleep(100);
}

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================

// main setup
void setup() {
    setupPins();
    setupDisplay();
    
    setupSidetoneTimer3();

            // 0123456789ABCDEF
    lcd.print("seconds spent :");
    Serial.begin(115200);
    Serial.print("seconds spent :");
    
    mySCoop.start();
}

// main loop
bool oldtransmit = true; // display fostamin    
char ch = 'x';
int oldstate = 99; // display fostamin
void loop() {
    yield();

    if (digitalRead(dit_in) == LOW) {
        transmit = true;
    }
    if (digitalRead(dah_in) == LOW) {
        transmit = false;
    }

    if (oldtransmit != transmit) {
        lcd.setCursor(0, 1);
        lcd.print(transmit ? "*" : "O");
        oldtransmit = transmit;
    }
    
    if (oldstate != txState) {
        switch (txState) {
            case TXS_IDLE_SILENT:
                ch = 'i';
                break;
            case TXS_ENABLE_RAMPUP:
                ch = 'u';
                break;
            case TXS_ON:
                ch = 'o';
                break;
            case TXS_DISABLE_RAMPDOWN:
                ch = 'd';
                break;
        }
        lcd.setCursor(1, 1);
        lcd.print(ch);
        lcd.print(' ');
        lcd.print(rampCycles);
        lcd.print(' ');
        lcd.print(volume);
        lcd.print(' ');
        oldstate = txState;
    }    
}

