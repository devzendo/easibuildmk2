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

// input change to event conversion, called by inputPinChange
uint16_t oldEventPins = 0;
uint16_t newEventPins = 0;
uint16_t lastDah = ~dahBit;
uint16_t lastDit = ~ditBit;
uint16_t lastBtn = ~btnBit;
void eventDecode(uint16_t changedPins) {
    // do the decoding, and call eventOccurred with each event found...
    // definitely need debouncing on the dit/dah, not so bad on encoder button.
    uint16_t currPin = changedPins & dahBit;
    if (currPin != lastDah) {
        eventOccurred(evDah | (lastDah ? evOn : evOff));
        lastDah = currPin;
    }

    currPin = changedPins & ditBit;
    if (currPin != lastDit) {
        eventOccurred(evDit | (lastDit ? evOn : evOff));
        lastDit = currPin;
    }
        
    currPin = changedPins & btnBit;
    if (currPin != lastBtn) {
        eventOccurred(evBtn | (lastBtn ? evOn : evOff));
        lastBtn = currPin;
    }
    // rotary encoder....
}

#endif

