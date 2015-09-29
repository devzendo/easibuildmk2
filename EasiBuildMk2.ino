// EasiBuild Mk 2 Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//
// For Arduino Micro. If you want to target a different board, search for the comments 'CHANGE_BOARD';
// if you want to change the pins used for the various inputs/outputs, search for the comments 'CHANGE_PIN'.
//
// Acknowledgements:
// SCoop: Fabrice Oudert http://forum.arduino.cc/index.php?topic=137801.0
//
// The sketch can be built to perform two functions:
// 1. The normal EasiBuild Mk 2 firmware
// 2. A control board hardware test harness
// "Out of the box", as checked into the repository, you get 1. If you want to
// build 2, uncomment this line:
#define TEST_HARNESS



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

#ifdef TEST_HARNESS
#include "TestHarness.h"
#else
#include "Transceiver.h"
#endif


