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
    { "DDS Update", ddsUpdateOut },
    { "DDS Clock ", ddsClockOut }
};


void refreshDisplay() {
    lcd.setCursor(0, 1);
    lcd.print(outputs[which].name);
    Serial.println(outputs[which].name);
}


int autoloop = 0;
defineTaskLoop(Task1) {
            // 0123456789ABCDEF
            // DDS Update *
    int thisWhich = which;
    lcd.setCursor(11, 1);
    lcd.print('*');
    digitalWrite(13, HIGH);
//    Serial.print("writing ");
    for (int i=0; i<maxOutputs; i++) {
//        Serial.print(i);
//        Serial.print(' ');
//        Serial.print(i == thisWhich ? HIGH : LOW);
//        Serial.print(", ");
        digitalWrite(outputs[i].pinNo, i == thisWhich ? HIGH : LOW);
    }
//    Serial.println("");
    sleep(1000);

    digitalWrite(13, LOW);
    lcd.setCursor(11, 1);
    lcd.print(' ');
//    Serial.print("writing ");
//    Serial.print(thisWhich);
//    Serial.println(" low");
    digitalWrite(thisWhich, LOW);
    sleep(1000);
    
    if (autoloop == 10) {
        autoloop = 0;
        if (which == maxOutputs - 1) {
            which = 0;
        } else {
            which++;
        }
        refreshDisplay();
    } else {
        autoloop++;
    }
    
}

defineTaskLoop(Task2) {
    int event;
    bool changed = false;
    if (eventFifo.get(&event)) {
        Serial.print("<ev:");
        Serial.println(event);
        if (event == (evLeft | evSlow)) {
            changed = true;
            if (which == 0) {
                which = maxOutputs - 1;
            } else {
                which--;
            }
        } else if (event == (evRight | evSlow)) {
            changed = true;
            if (which == maxOutputs - 1) {
                which = 0;
            } else {
                which++;
            }
        }
    }
    if (changed) {
//        Serial.println(outputs[which].name);
        refreshDisplay();
    }
    yield();
}

// main setup
void setup() {
    which = 0;
    setupPins();
    setupDisplay();
    Serial.begin(115200);

    mySCoop.start();

    setupSidetoneTimer3();

    lcd.setCursor(0, 0);
            // 0123456789ABCDEF
    lcd.print("Choose O/P  <> ");
    lcd.setCursor(0, 1);
    lcd.print("               ");

    refreshDisplay();
}

// main loop
void loop() {
    yield();
}

#endif

