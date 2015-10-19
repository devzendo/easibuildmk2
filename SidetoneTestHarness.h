// EasiBuild Mk 2 Test Harness Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _SIDETONE_TEST_HARNESS_H_
#define _SIDETONE_TEST_HARNESS_H_

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================

bool volSelected = true;

void refreshDisplay() {
    char buf[20];
    // 0123456789ABCDEF
    // Freq  BTN  Vol
    // 1000 Hz <>  10
    lcd.setCursor(5, 0);
    lcd.print(volSelected ? ' ' : '<');
    lcd.setCursor(9, 0);
    lcd.print(volSelected ? '>' : ' ');
    sprintf(buf, "%-3d Hz  <>   %2d ", (int)frequency, (int)(volume * 10));
    lcd.setCursor(0, 1);
    lcd.print(buf);
}

defineTaskLoop(Task1) {
    int event;
    if (eventFifo.get(&event)) {
        if (event == (evBtn | evOff)) {
            volSelected = !volSelected;
        } else if (event == evLeft | evFast) {
            if (volSelected) {
                setSidetoneVolume(max(0.0, volume - 0.01));
            } else {
                setSidetoneFrequency(max(400.0, frequency - 1));
            }
        } else if (event == evRight | evFast) {
            if (volSelected) {
                setSidetoneVolume(min(1.0, volume + 0.01));
            } else {
                setSidetoneFrequency(min(800.0, frequency + 1));
            }
        }
    }
    yield();
}

// Done as a separate task, since rapid event generation from the rotary encoder causes the slow display refresh to buffer.
defineTaskLoop(Task2) {
    refreshDisplay();
    sleep(250);
}

// main setup
void setup() {
    setupPins();
    setupDisplay();
    mySCoop.start();

    setupSidetoneTimer3();

    lcd.setCursor(0, 0);
            // 0123456789ABCDEF
    lcd.print("Freq  BTN   Vol ");
            // 1000 Hz <>   10
    refreshDisplay();
}

// main loop
void loop() {
    yield();
}

#endif

