// EasiBuild Mk 2 Transceiver Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//

#ifndef _TRANSCEIVER_H_
#define _TRANSCEIVER_H_

#include "Menu.h"

class NumberedList immediateMenu = {
   8, 
   {
       "Freq. lock", 
       "R.I.T.",
       "Send macro 1",
       "Send macro 2",
       "Send macro 3",
       "Send macro 4",
       "Send macro 5",
       "Tune",
   }
};

class NumberedList setupMenu = {
    8,
    {
        "Filter 1",
        "Filter 2",
        "Filter 3",
        "CW delay",
       //1234567890123456  
        "CW filt. freq.",
        "CW key type",
        "Farnsworth spd.",
        "Character spd.",
        "Paddle polarity",
        "Edit macro 1",
        "Edit macro 2",
        "Edit macro 3",
        "Edit macro 4",
        "Edit macro 5",
        "Reset settings",
        "Sidetone pitch",
       //1234567890123456  
        "Sidetone volume",
        "Version"
    }
};

const int modeOperator = 0;
const int modeImmediate = 1;
const int modeSetup = 2;
volatile int mode;

volatile double ddsFrequency;
double ddsDigitDelta;
double lowerBandBound;
double upperBandBound;

void refreshOperator() {
}

void refreshImmediate() {
}

void refreshSetup() {
}


void refresh() {
    switch (mode) {
        case modeOperator: refreshOperator(); break;
        case modeImmediate: refreshImmediate(); break;
        case modeSetup: refreshSetup(); break;
    }
}

void enterOperator() {
}

void enterImmediate() {
}

void enterSetup() {
}

void switchMode(int newMode) {
    mode = newMode;
    switch (mode) {
        case modeOperator: enterOperator(); break;
        case modeImmediate: enterImmediate(); break;
        case modeSetup: enterSetup(); break;
    }
    refresh();
}

void eventOperator(int event) {
//    if (event & evLeft) {
        
}

void eventImmediate(int event) {
}

void eventSetup(int event) {
}

//==============================================================================
//=== SCOOP TASKS
//==============================================================================

defineTaskLoop(Task1) {
    int event;
    if (eventFifo.get(&event)) {
        switch (mode) {
            case modeOperator: eventOperator(event); break;
            case modeImmediate: eventImmediate(event); break;
            case modeSetup: eventSetup(event); break;
        }
    }
    yield();
}

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================

char *version="0.0"; // only ever x.x, more requires rework below...

// Also called by factory reset
void initialise() {
    char buf[20];
    setupPins();
    setupSidetoneTimer3();

    lcd.setCursor(0, 0);
            // 0123456789ABCDEF
    lcd.print("EasiBuild Mk 2  ");
    lcd.setCursor(0, 1);
    sprintf(buf, "v%s, M0CUV  ", version);
    lcd.print(buf);
    
    // start timer for 1sec to switch to next mode
    // check nvram checksum
    // if wrong, reset to sane, compute checksum, save, set to goto setup mode
    // if ok, set to goto operator mode
    // load all runtime settings from nvram area
    // if filters set up
    //    set up band filter switch
    // if frequency set up
    //    set up dds
    ddsFrequency = 0.0;
    ddsDigitDelta = 1.0; // TODO store 
    lowerBandBound = 14000000.0;
    upperBandBound = 14350000.0;
    // goto determined mode
    switchMode(modeOperator);
}

// main setup

void setup() {
    setupDisplay();
    Serial.begin(115200);
    mySCoop.start();

    initialise();
}

void loop() {
    yield();
}

#endif

