// EasiBuild Mk 2 Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//
// For Arduino Micro. If you want to target a different board, search for the comments 'CHANGE_BOARD';
// if you want to change the pins used for the various inputs/outputs, search for the comments 'CHANGE_PIN'.
//
// Acknowledgements:
// LiquidCrystal_I2C: Francisco Malpartida: https://arduino-info.wikispaces.com/LCD-Blue-I2C

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

// CHANGE_BOARD
#include <Arduino.h> 

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//==============================================================================
//=== LCD DISPLAY
//==============================================================================

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setupDisplay() {
    lcd.begin(16, 2);
    lcd.backlight();
    lcd.setCursor(0, 0);
}

#endif

