// EasiBuild Mk 2 Test Harness Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _DDS_TEST_HARNESS_H_
#define _DDS_TEST_HARNESS_H_

#include "DDS.h"

DDS dds;

typedef struct Band {
    const Hz lowBoundHz;
    const Hz highBoundHz;
    const char *name;
    Hz currHz;
} Band;
    
const int maxBands = 9;
Band bands[maxBands] = {
    {  1810000L,  2000000L, "160m",   1810000L },
    {  3500000L,  3800000L, "80m ",   3500000L },
    {  7000000L,  7200000L, "40m ",   7000000L },
    { 10100000L, 10150000L, "30m ",  10100000L },
    { 14000000L, 14350000L, "20m ",  14060000L },
    { 18068000L, 18168000L, "17m ",  18068000L },
    { 21000000L, 21450000L, "15m ",  21000000L },
    { 24890000L, 24990000L, "12m ",  24890000L },
    { 28000000L, 29700000L, "10m ",  28000000L }
};
int currBand = 4; // let's get some DX!

void displayBand() {
    lcd.setCursor(0, 0);
    lcd.print(bands[currBand].name);
}

char buffer[50];
void display() {
    lcd.setCursor(0, 1);
//[ 1800000]
// 0123456789012345
              //     0123456789012345678901234567890
    sprintf(buffer, "%8ld", bands[currBand].currHz);
    lcd.write(buffer, 3);
    lcd.print('.');
    lcd.write(buffer + 3, 3);
    lcd.print('.');
    lcd.write(buffer + 6, 2);
    lcd.print(" MHz");
}

void setDDSFrequency() {
    dds.setDDSFrequency(bands[currBand].currHz);
}

void processEvent(int eventCode) {
    bool change = false;
    if (eventCode == (evBtn | evOff)) {
        if (++currBand == maxBands) {
            currBand = 0;
        }
        displayBand();
        change = true;
    }
    if (eventCode == (evLeft | evFast)) {
        if (bands[currBand].currHz > bands[currBand].lowBoundHz) {
            bands[currBand].currHz--;
            change = true;
        }
    }
    if (eventCode == (evRight | evFast)) {
        if (bands[currBand].currHz < bands[currBand].highBoundHz) {
            bands[currBand].currHz++;
            change = true;
        }
    }
    if (change) {
        display();
        setDDSFrequency();
    }
}


defineTaskLoop(Task2) {
    int event;
    if (eventFifo.get(&event)) {
        processEvent(event);
    }
    yield();
}

// main setup
void setup() {
    setupPins();
    setupDisplay();

    mySCoop.start();

    setupSidetoneTimer3();

    dds.resetDDS();
    
    currBand = 4;
    lcd.clear();
    displayBand();
    display();
    setDDSFrequency();
    
    Serial.begin(115200);
}

// main loop
void loop() {
    yield();
}



#endif 
