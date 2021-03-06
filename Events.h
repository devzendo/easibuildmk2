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

const int evOff        = 0x00; // all buttons/key/paddle
const int evOn         = 0x01; // all buttons/key/paddle
const int evMedium     = 0x02; // button only medium press
const int evHard       = 0x03; // button only longer press
const int evStateMask  = evOff | evOn | evMedium | evHard;

const int evDah        = 0x80; // off, on
const int evDit        = 0x40; // off, on
const int evBtn        = 0x20; // off, on, medium, hard
const int evButtonMask = evDah | evDit | evBtn;

const int evLeft       = 0x10;
const int evRight      = 0x08;
const int evEncoderMask = evLeft | evRight;

const int evFast       = 0x01; // rotary encoder normal events
const int evSlow       = 0x02; // rotary envoder damped events
const int evSpeedMask  = evFast | evSlow;

const int evTimerTick  = 0x04;

const int evTypeMask   = evDah | evDit | evBtn | evLeft | evRight | evTimerTick;

defineFifo(eventFifo, int, 100)

defineTimerRun(eventQueueingTimer, 0) {
    static int tickEvent = evTimerTick;
    eventFifo.put(&tickEvent);
}

static char zut[20];
inline void eventOccurred(int eventCode) {
    sprintf(zut, ">ev:0x%04X", eventCode);
    Serial.println(zut);
    if (!eventFifo.put(&eventCode)) {
        Serial.println("FIFO overrun");
    }
}

class Damper {
public:
    Damper(int tickDelay) {
        delayLimit = tickDelay;
        reset();
    }

    void reset() {
        delay = delayLimit >> 1;
    }
    
    bool left() {
        if (delay == 0) {
            delay = delayLimit;
            return true;
        } else {
            delay--;
            return false;
        }
    }
    
    bool right() {
        if (delay == delayLimit) {
            delay = 0;
            return true;
        } else {
            delay++;
            return false;
        }
    }
private:
    int delayLimit;
    int delay;
};

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
    // called every checkMsec.
    // The key state is +5v=released, 0v=pressed; there are pullup resistors.
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
Damper damper(10);
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
        bool pressed = !btnDebounce.keyReleased;
        pressDurationDetector.keyStateChanged(pressed);
        if (pressed) {
            damper.reset();
        }
        eventOccurred(evBtn | (pressed ? evOn : evOff));
    }

    // rotary encoder....
    encoderValue = encoderValue << 2;
    encoderValue = encoderValue | ((rawPins & (enclBit | encrBit)) >> 5);
    switch (encoderLookupTable[encoderValue & 0b1111]) {
        case 0:
            break;
        case 1:
            eventOccurred(evLeft | evFast);
            if (damper.left()) {
                eventOccurred(evLeft | evSlow);
            }
            break;
        case -1:
            eventOccurred(evRight | evFast);
            if (damper.right()) {
                eventOccurred(evRight | evSlow);
            }
            break;
    }
}

#endif

