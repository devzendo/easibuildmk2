// EasiBuild Mk 2 Firmware: event definitions
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//

const int evStateMask = 0x01;
const int evOn = 0x01;
const int evOff = 0x00;

const int evDah = 0x80;
const int evDit = 0x40;
const int evBtn = 0x20;

const int evVelocityMask = 0x07;
const int evLeft = 0x10;
const int evRight = 0x08;

// forward declaration
void eventOccurred(int eventCode);

// input change to event conversion, called by inputPinChange
uint16_t oldEventPins = 0;
uint16_t newEventPins = 0;

void eventDecode(uint16_t changedPins) {
    // do the decoding, and call eventOccurred with each event found...
}

#endif

