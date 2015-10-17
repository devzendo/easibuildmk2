// EasiBuild Mk 2 Test Harness Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _INPUTS_TEST_HARNESS_H_
#define _INPUTS_TEST_HARNESS_H_

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================

// main setup
void setup() {
    setupPins();
    setupDisplay();
    
            // 0123456789ABCDEF
    lcd.print(".LRB-???       ");
}

// main loop
uint16_t oldPins = 0xffff; // display fostamin
uint16_t newPins = 0;
uint16_t bit;
int bufidx;
char buffer[20];
int countx = 0;
void loop() {

    newPins = (PINB & 0xF0) | ((PIND & 0x40) >> 3);

    if (oldPins != newPins) {
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
       oldPins = newPins;
    }
}


#endif

