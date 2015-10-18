// EasiBuild Mk 2 Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//
// Acknowledgements:
// SCoop: Fabrice Oudert http://forum.arduino.cc/index.php?topic=137801.0
//
// For Arduino Micro. If you want to target a different board, search for the comments 'CHANGE_BOARD';
// if you want to change the pins used for the various inputs/outputs, search for the comments 'CHANGE_PIN'.
//
// The sketch can be built to perform two functions:
// 1. The normal EasiBuild Mk 2 firmware
// 2. A control board hardware test harness
// "Out of the box", as checked into the repository, you get 1. If you want to
// build one of the test harnesses, uncomment the appropriate line:

//#define TRANSCEIVER

// Displays all input pins as a bit in a binary/hex number with change count.
//#define INPUTS_TEST_HARNESS

// Plays the sidetone, allows frequency and volume adjustment.
#define SIDETONE_TEST_HARNESS

// Toggles an output pin once per second. Press button to switch to next output pin.
//#define OUTPUTS_TEST_HARNESS

// Shows interrupt handler input pin conversion to events on the serial output.
//#define EVENTS_TEST_HARNESS

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// CHANGE_BOARD
#include <Arduino.h> 

#include <SCoop.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "PinAssignments.h"
#include "Events.h"
#include "SidetoneOscillator.h"
#include "Display.h"

#ifdef INPUTS_TEST_HARNESS
#include "InputsTestHarness.h"
#endif

#ifdef SIDETONE_TEST_HARNESS
#include "SidetoneTestHarness.h"
#endif

#ifdef OUTPUTS_TEST_HARNESS
#include "OutputsTestHarness.h"
#endif

#ifdef EVENTS_TEST_HARNESS
#include "EventsTestHarness.h"
#endif

#ifdef TRANSCEIVER
#include "Transceiver.h"
#endif

