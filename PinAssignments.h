// EasiBuild Mk 2 Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//
// For Arduino Micro. If you want to target a different board, search for the comments 'CHANGE_BOARD';
// if you want to change the pins used for the various inputs/outputs, search for the comments 'CHANGE_PIN'.

#ifndef _PIN_ASSIGNMENTS_H_
#define _PIN_ASSIGNMENTS_H_

// CHANGE_PINS
// Right-hand side of Arduino Micro...
// Input pins are all on PINB or PIND for rapid reading in the interrupt service routine.
const int dahIn = 12; // PIND
const int dahBit = 0x08;

const int ditIn = 11; // PINB
const int ditBit = 0x80;

const int encrIn = 10;// PINB
const int encrBit = 0x20;

const int enclIn = 9; // PINB
const int enclBit = 0x40;

const int btnIn = 8;  // PINB
const int btnBit = 0x10;

const int band1Out = 7;
const int rxtxOut = 6;
const int sidetoneOut = 5;  // PWM, with RC low-pass filter network to convert
                            // to analogue. On OCR3A, PWM phase correct.
const int keyOut = 4;
// display is on 2 and 3, i2c.

// Left-hand side of Arduino Micro...
const int band2Out = 19;
const int band3Out = 18;
const int ddsClockOut = 14;
const int ddsUpdateOut = 15;
const int ddsDataOut = 16;
const int ddsResetOut = 17;

inline uint16_t readPins() {
    return (PINB & 0xF0) | ((PIND & 0x40) >> 3);
}

// input change detection, called in loop() for test harness, or in ISR
uint16_t oldPins;
uint16_t newPins;
uint16_t initialPins;

void setupPins() {
    pinMode(dahIn, INPUT_PULLUP);
    pinMode(ditIn, INPUT_PULLUP);
    pinMode(encrIn, INPUT_PULLUP);
    pinMode(enclIn, INPUT_PULLUP);
    pinMode(btnIn, INPUT_PULLUP);
    
    pinMode(band1Out, OUTPUT);
    pinMode(band2Out, OUTPUT);
    pinMode(band3Out, OUTPUT);

    pinMode(rxtxOut, OUTPUT); 
    pinMode(keyOut, OUTPUT);
    
    pinMode(ddsClockOut, OUTPUT);
    pinMode(ddsUpdateOut, OUTPUT);
    pinMode(ddsDataOut, OUTPUT);
    pinMode(ddsResetOut, OUTPUT);

    initialPins = oldPins = newPins = readPins();
}

// forward declaration
void inputPinChange(uint16_t changedPins);

void inputSense() {
    newPins = readPins();
    if (oldPins != newPins) {
        oldPins = newPins;
        inputPinChange(newPins);
    }
}

#endif

