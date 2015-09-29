// EasiBuild Mk 2 Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//
// For Arduino Micro. If you want to target a different board, search for the comments 'CHANGE_BOARD';
// if you want to change the pins used for the various inputs/outputs, search for the comments 'CHANGE_PIN'.

#ifndef _PIN_ASSIGNMENTS_H_
#define _PIN_ASSIGNMENTS_H_

// CHANGE_PINS
// Right-hand side of Arduino Micro...
// Input pins are all on PINB for rapid reading in the pin change interrupt
// service routine.
const int dah_in = 12;
const int dit_in = 11;
const int encr_in = 10;
const int encl_in = 9;
const int btn_in = 8;
const int band1_out = 7;
const int rxtx_out = 6;
const int sidetone_out = 5; // PWM, with RC low-pass filter network to convert
                            // to analogue. On OCR3A, PWM phase correct.
const int key_out = 4;
// display is on 2 and 3, i2c.

// Left-hand side of Arduino Micro...
const int band2_out = 19;
const int band3_out = 18;
const int dds_clock_out = 14;
const int dds_update_out = 15;
const int dds_data_out = 16;
const int dds_reset_out = 17;

void setupPins() {
    pinMode(dah_in, INPUT_PULLUP);
    pinMode(dit_in, INPUT_PULLUP);
    pinMode(encr_in, INPUT_PULLUP);
    pinMode(encl_in, INPUT_PULLUP);
    pinMode(btn_in, INPUT_PULLUP);
    
    pinMode(band1_out, OUTPUT);
    pinMode(band2_out, OUTPUT);
    pinMode(band3_out, OUTPUT);

    pinMode(rxtx_out, OUTPUT); 
    pinMode(key_out, OUTPUT);
    
    pinMode(dds_clock_out, OUTPUT);
    pinMode(dds_update_out, OUTPUT);
    pinMode(dds_data_out, OUTPUT);
    pinMode(dds_reset_out, OUTPUT);
}

#endif

