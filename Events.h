// EasiBuild Mk 2 Firmware: event definitions
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//

#ifndef _EVENTS_H_
#define _EVENTS_H_

const int evOn = 0x01;
const int evOff = 0x00;
const int evStateMask = evOn | evOff;

const int evDah = 0x80;
const int evDit = 0x40;
const int evBtn = 0x20;
const int evButtonMask = evDah | evDit | evBtn;

const int evVelocityMask = 0x07;
const int evLeft = 0x10;
const int evRight = 0x08;
const int evTypeMask = evDah | evDit | evBtn | evLeft | evRight;

// forward declaration
void eventOccurred(int eventCode);

// Based on code by Jack Ganssle.
const uint8_t checkMsec = 4;     // Read hardware every so many milliseconds
const uint8_t pressMsec = 10;    // Stable time before registering pressed
const uint8_t releaseMsec = 100; // Stable time before registering released

class Debouncer {
public:
    // called every checkMsec 
    void debounce(bool rawPinState) {
        bool rawState;
        keyChanged = false;
        keyPressed = debouncedKeyPress;
        if (rawPinState == debouncedKeyPress) {
            // Set the timer which allows a change from current state
            resetTimer();
        } else {
            // key has changed - wait for new state to become stable
            debouncedKeyPress = rawPinState;
            keyChanged = true;
            keyPressed = debouncedKeyPress;
            // And reset the timer
            resetTimer();
        }
    }

    // Signals the key has changed from open to closed, or the reverse.
    bool keyChanged;
    // The current debounced state of the key.
    bool keyPressed;

private:
    void resetTimer() {
        if (debouncedKeyPress) {
            count = releaseMsec / checkMsec;
        } else {
            count = pressMsec / checkMsec;
        }
    }

    uint8_t count = releaseMsec / checkMsec;
    // This holds the debounced state of the key.
    bool debouncedKeyPress = false; 
};

Debouncer ditDebounce;
Debouncer dahDebounce;
Debouncer btnDebounce;

// input change to event conversion, called in loop or ISR
void eventDecode(uint16_t rawPins) {
    // Do the decoding, and call eventOccurred with each event found...
    // Need to debounce dit/dah, not so bad on encoder button.
    ditDebounce.debounce(rawPins & ditBit);
    if (ditDebounce.keyChanged) {
        eventOccurred(evDit | (ditDebounce.keyPressed ? evOff : evOn));
    }
    dahDebounce.debounce(rawPins & dahBit);
    if (dahDebounce.keyChanged) {
        eventOccurred(evDah | (dahDebounce.keyPressed ? evOff : evOn));
    }
    btnDebounce.debounce(rawPins & btnBit);
    if (btnDebounce.keyChanged) {
        eventOccurred(evBtn | (btnDebounce.keyPressed ? evOff : evOn));
    }
    // rotary encoder....
}

#endif

