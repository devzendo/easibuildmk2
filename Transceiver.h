// EasiBuild Mk 2 Transceiver Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//

#ifndef _TRANSCEIVER_H_
#define _TRANSCEIVER_H_

#include "Controllers.h"
#include "Menu.h"


class FakeSetupMode: public Controller {
    char *name() {
        return "FSM";
    }
    void enter() {
        lcd.setCursor(0, 0);
                // 0123456789ABCDEF
        lcd.print("Setup     mode  ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
    }
    void exit() {
    }
    
    void processEvent(int event) {
        if (event & (evHard | evMedium) != 0) {
            controllerStack.pop();
        }
    }

} fakeSetupMode;

class FakeImmediateMode: public Controller {
    char *name() {
        return "FIM";
    }
    void enter() {
        lcd.clear();
        lcd.setCursor(0, 0);
                // 0123456789ABCDEF
        lcd.print("Immediate mode  ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
    }
    void exit() {
    }
    
    void processEvent(int event) {
        if ((event & evHard) == evHard) {
            controllerStack.push(&fakeSetupMode);
            return;
        }
        if ((event & (evHard | evMedium)) != 0) {
            controllerStack.pop();
        }
    }

} fakeImmediateMode;

class FakeOperatingMode: public Controller {
    char *name() {
        return "FOM";
    }

    void enter() {
        lcd.clear();
        lcd.setCursor(0, 0);
                // 0123456789ABCDEF
        lcd.print("14,060.00 MHz   ");
        lcd.setCursor(0, 1);
        lcd.print("RX            P ");
    }
    void exit() {
    }
    
    void processEvent(int event) {
        if (event & evMedium == evMedium) {
            controllerStack.push(&fakeImmediateMode);
        }
    }

} fakeOperatingMode;

char *version="0.0"; // only ever x.x, more requires rework below...
class WelcomeController: public Controller {
    char *name() {
        return "WEL";
    }

    void enter() {
        char buf[20];
        lcd.clear();
        lcd.setCursor(0, 0);
                // 0123456789ABCDEF
        lcd.print("EasiBuild Mk 2  ");
        lcd.setCursor(0, 1);
        sprintf(buf, "v%s, M0CUV  ", version);
        lcd.print(buf);

        eventQueueingTimer.schedule(2000, 1);
    }
    
    void exit() {
    }
    
    void processEvent(int event) {
        if ((event & evTimerTick) || (event & evBtn)) {
          Serial.println("wel processing event");
//        eventQueueingTimer.schedule(0); // stop timer
          controllerStack.pop();
        }
    }

} welcomeController;

volatile double ddsFrequency;
double ddsDigitDelta;
double lowerBandBound;
double upperBandBound;

/*
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
*/
//==============================================================================
//=== SCOOP TASKS
//==============================================================================

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================

// Also called by factory reset
void initialise() {
    char buf[20];
    Serial.println("initialising pins");
    setupPins();
    Serial.println("initialising controllers");

    controllerStack.reset();
    Serial.println("initialising sidetone timer");

    setupSidetoneTimer3();
    
    Serial.println("push fom");
    controllerStack.push(&fakeOperatingMode);
    Serial.println("push wel");
    controllerStack.push(&welcomeController);
    
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
    //switchMode(modeOperator);
}

// main setup

void setup() {
    setupDisplay();
    Serial.begin(115200);
    for (int i=0; i<50; i++) {
        Serial.println("waiting 5 seconds");
        sleep(100);
    }
    
    Serial.println("setup scoop");
    mySCoop.start();

    initialise();
}

void loop() {
    yield();
}

#endif

