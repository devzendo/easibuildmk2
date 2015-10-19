// EasiBuild Mk 2 Test Harness Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _EVENTS_TEST_HARNESS_H_
#define _EVENTS_TEST_HARNESS_H_

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================
char buffer[50];
int countx = 0;
void decodeEventToSerial(int eventCode) {
                                      //     XXX ....
              //     0123456789012345678901234567890
    sprintf(buffer, "Cnt: 0x%04X Evt: 0x%04X ", countx++, eventCode);
    switch (eventCode & evTypeMask) {
        case evDit : strcpy(buffer + 24, "DIT"); break;
        case evDah : strcpy(buffer + 24, "DAH"); break;
        case evBtn : strcpy(buffer + 24, "BTN"); break;
        case evLeft : strcpy(buffer + 24, "<<<"); break;
        case evRight : strcpy(buffer + 24, ">>>"); break;
        default: strcpy(buffer + 24, "???"); break;
    }
    buffer[27] = ' ';
    if (eventCode & evButtonMask) {
        switch (eventCode & evStateMask) {
            case evOff: strcpy(buffer + 28, "OFF"); break;
            case evOn: strcpy(buffer + 28, "ON"); break;
            case evMedium: strcpy(buffer + 28, "MEDIUM"); break;
            case evHard: strcpy(buffer + 28, "HARD"); break;
            default: strcpy(buffer + 28, "???"); break;
        }
    } else {
        switch (eventCode & evSpeedMask) {
            case evFast: strcpy(buffer + 28, "FAST"); break;
            case evSlow: strcpy(buffer + 28, "SLOW"); break;
            default: strcpy(buffer + 28, "???"); break;
        }
    }
    Serial.println(buffer);
}

defineTaskLoop(Task1) {
    digitalWrite(13, HIGH);
    sleep(100);
    digitalWrite(13, LOW);
    sleep(100);
}

defineTaskLoop(Task2) {
    int event;
    if (eventFifo.get(&event)) {
        decodeEventToSerial(event);
    }
    yield();
}

// main setup
void setup() {
    setupPins();
    setupDisplay();

    mySCoop.start();

    setupSidetoneTimer3();

            // 0123456789ABCDEF
    lcd.print("Events > Console");
    Serial.begin(115200);
}

// main loop
void loop() {
    yield();
}

#endif

