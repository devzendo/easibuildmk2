// EasiBuild Mk 2 Test Harness Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _INPUTS_TEST_HARNESS_H_
#define _INPUTS_TEST_HARNESS_H_

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================

uint16_t bit;
int bufidx;
char buffer[20];
int countx = 0;

void inputPinChange(uint16_t changedPins) {
    lcd.setCursor(9, 0);
    sprintf(buffer, "0x%04X", countx++);
    lcd.print(buffer);
       
    lcd.setCursor(0, 1);
    bit = 0x0080;
    bufidx = 0;
    while (bit != 0) {
        buffer[bufidx++] = (newPins & bit) == 0 ? '0' : '1';
        bit >>= 1;
    }
       
    sprintf(buffer + bufidx, " 0x%04X", newPins);
    lcd.print(buffer);
}

// main setup
void setup() {
    setupPins();
    setupDisplay();
    
            // 0123456789ABCDEF
    lcd.print(".LRB-???       ");
    // initialise as if something has changed
    oldPins = ~newPins;
    inputPinChange(initialPins);
}


// main loop
void loop() {
    newPins = readPins();
    if (oldPins != newPins) {
        oldPins = newPins;
        inputPinChange(newPins);
    }
}


#endif

