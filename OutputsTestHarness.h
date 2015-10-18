// EasiBuild Mk 2 Test Harness Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _OUTPUTS_TEST_HARNESS_H_
#define _OUTPUTS_TEST_HARNESS_H_

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================
volatile int which = 0;

struct whichData {
    char *name;
    int pinNo;
};

const int maxOutputs = 9;
struct whichData outputs[maxOutputs] = {
    { "Band 1    ", band1Out },
    { "Band 2    ", band2Out },
    { "Band 3    ", band3Out },
    { "RX/TX     ", rxtxOut },
    { "Key       ", keyOut },
    { "DDS Reset ", ddsResetOut },
    { "DDS Data  ", ddsDataOut },
    { "DDS Clock ", ddsClockOut },
    { "DDS Update", ddsUpdateOut }
};

defineTaskLoop(Task1) {
            // 0123456789ABCDEF
            // DDS Update *
    int thisWhich = which;
    lcd.setCursor(11, 1);
    lcd.print('*');
    for (int i=0; i<maxOutputs; i++) {
        digitalWrite(outputs[i].pinNo, i == thisWhich ? HIGH : LOW);
    }
    sleep(100);

    lcd.setCursor(11, 1);
    lcd.print(' ');
    digitalWrite(thisWhich, LOW);
    sleep(100);
}

void refreshDisplay() {
    lcd.setCursor(0, 1);
    lcd.print(outputs[which].name);
}

class Damper {
public:
    bool left() {
        if (delay == 0) {
            delay = 10;
            return true;
        } else {
            delay--;
            return false;
        }
    }
    
    bool right() {
        if (delay == 10) {
            delay = 0;
            return true;
        } else {
            delay++;
            return false;
        }
    }
private:
    int delay = 10;
};

Damper damp;
defineTaskLoop(Task2) {
    int event;
    bool changed = false;
    if (eventFifo.get(&event)) {
        if (event == evLeft) {
            if (damp.left()) {
                changed = true;
                if (which == 0) {
                    which = maxOutputs - 1;
                } else {
                    which--;
                }
            }
        } else if (event == evRight) {
            if (damp.right()) {
                changed = true;
                if (which == maxOutputs - 1) {
                    which = 0;
                } else {
                    which++;
                }
            }
        }
    }
    if (changed) {
        refreshDisplay();
    }
    yield();
}

// main setup
void setup() {
    setupPins();
    setupDisplay();

    mySCoop.start();

    setupSidetoneTimer3();

    lcd.setCursor(0, 0);
            // 0123456789ABCDEF
    lcd.print("Choose O/P  <> ");
    lcd.setCursor(0, 1);
    lcd.print("               ");
    which = 0;
    refreshDisplay();
}

// main loop
void loop() {
    yield();
}

#endif

