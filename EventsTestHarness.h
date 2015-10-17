// EasiBuild Mk 2 Test Harness Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _EVENTS_TEST_HARNESS_H_
#define _EVENTS_TEST_HARNESS_H_

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================

// main setup
void setup() {
    setupPins();
    setupDisplay();
    
            // 0123456789ABCDEF
    lcd.print("Events > Console");
    Serial.begin(115200);
}

char buffer[40];
int countx = 0;

void eventOccurred(int eventCode) {
    sprintf(buffer, "Cnt: 0x%04X Evt: 0x%04X", countx++, eventCode);
    // actually display the event details...
}

void inputPinChange(uint16_t changedPins) {
    eventDecode(changedPins);
}

// main loop
void loop() {
    inputSense();
}


#endif

