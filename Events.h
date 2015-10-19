// EasiBuild Mk 2 Firmware: event definitions
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//
// Acknowledgements: Jack Ganssle for his article on firmware debouncing
// http://www.ganssle.com/debouncing-pt2.htm
//
// Rotary encoder reading from an article at
// http://makeatronics.blogspot.co.uk/2013/02/efficiently-reading-quadrature-with.html


#ifndef _EVENTS_H_
#define _EVENTS_H_

const int evOff = 0x00;
const int evOn = 0x01;
const int evMedium = 0x02;
const int evHard = 0x03;
const int evStateMask = evOff | evOn | evMedium | evHard;

const int evDah = 0x80; // off, on
const int evDit = 0x40; // off, on
const int evBtn = 0x20; // off, on, medium, hard
const int evButtonMask = evDah | evDit | evBtn;

const int evLeft = 0x10;
const int evRight = 0x08;
const int evTypeMask = evDah | evDit | evBtn | evLeft | evRight;

defineFifo(eventFifo, int, 100)

inline void eventOccurred(int eventCode) {
    eventFifo.put(&eventCode);
}

class PressDurationDetector {
public:
    // Called every checkMsec, to update internal timer count.
    // returns evOff if no press in progress, or
    // evMedium, evHard for longer presses
    int longPressDetected() {
        if (!keyState) {
            return evOff;
        }
        switch (ticksSincePressed++) {
            case 125: 
                return evMedium; // 500ms after press
            case 500:
                keyState = false; // to prevent overflow if button held in for a long time
                return evHard;   // 2000ms after press
        }
        return evOff;
    }
    
    // Called whenever the debounced key state changes.
    void keyStateChanged(bool pressed) {
        keyState = pressed;
        ticksSincePressed = 0;
    }

private:
    bool keyState = false;
    int ticksSincePressed = 0;
};

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
        keyReleased = debouncedKeyPress;
        if (rawPinState == debouncedKeyPress) {
            // Set the timer which allows a change from current state
            resetTimer();
        } else {
            // key has changed - wait for new state to become stable
            debouncedKeyPress = rawPinState;
            keyChanged = true;
            keyReleased = debouncedKeyPress;
            // And reset the timer
            resetTimer();
        }
    }

    // Signals the key has changed from open to closed, or the reverse.
    bool keyChanged;
    // The current debounced state of the key.
    bool keyReleased;

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
PressDurationDetector pressDurationDetector;
static int8_t encoderLookupTable[] = {
    0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0
};
static uint8_t encoderValue = 0;

// input change to event conversion, called in loop or ISR
void eventDecode(uint16_t rawPins) {
    // Do the decoding, and call eventOccurred with each event found...
    // Need to debounce dit/dah, not so bad on encoder button.
    ditDebounce.debounce(rawPins & ditBit);
    if (ditDebounce.keyChanged) {
        eventOccurred(evDit | (ditDebounce.keyReleased ? evOff : evOn));
    }

    dahDebounce.debounce(rawPins & dahBit);
    if (dahDebounce.keyChanged) {
        eventOccurred(evDah | (dahDebounce.keyReleased ? evOff : evOn));
    }

    switch (pressDurationDetector.longPressDetected()) {
        case evMedium: eventOccurred(evBtn | evMedium); break;
        case evHard: eventOccurred(evBtn | evHard); break;
    }
    btnDebounce.debounce(rawPins & btnBit);
    if (btnDebounce.keyChanged) {
        pressDurationDetector.keyStateChanged(!btnDebounce.keyReleased);
        eventOccurred(evBtn | (btnDebounce.keyReleased ? evOff : evOn));
    }

    // rotary encoder....
    encoderValue = encoderValue << 2;
    encoderValue = encoderValue | ((rawPins & (enclBit | encrBit)) >> 5);
    switch (encoderLookupTable[encoderValue & 0b1111]) {
        case 0: break;
        case 1: eventOccurred(evLeft); break;
        case -1: eventOccurred(evRight); break;
    }
}

#endif

