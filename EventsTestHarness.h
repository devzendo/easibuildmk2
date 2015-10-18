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
    }
    if (eventCode & evButtonMask) {
        buffer[27] = ' ';
        strcpy(buffer + 28, eventCode & evStateMask ? "ON" : "OFF");
    } else {
        buffer[27] = '\0';
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

