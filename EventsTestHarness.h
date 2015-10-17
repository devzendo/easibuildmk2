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

char buffer[50];
int countx = 0;

void eventOccurred(int eventCode) {   //     XXX ....
              //     0123456789012345678901234567890
    sprintf(buffer, "Cnt: 0x%04X Evt: 0x%04X ", countx++, eventCode);
    switch (eventCode & evTypeMask) {
        case evDit : strcpy(buffer + 24, "DIT"); break;
        case evDah : strcpy(buffer + 24, "DAH"); break;
        case evBtn : strcpy(buffer + 24, "BTN"); break;
        case evLeft : strcpy(buffer + 24, "<<<"); break;
        case evRight : strcpy(buffer + 24, ">>>"); break;
    }
    buffer[27] = ' ';
    if (eventCode & evButtonMask) {
        strcpy(buffer + 28, eventCode & evStateMask ? "ON" : "OFF");
    } else {
        sprintf(buffer + 28, "%d", eventCode & evVelocityMask);
    }
    Serial.println(buffer);
}

void inputPinChange(uint16_t changedPins) {
    eventDecode(changedPins);
}

// main loop
void loop() {
    inputSense();
}


#endif

