// EasiBuild Mk 2 Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//
// Acknowledgements:
// Beric Dunn K6BEZ for the original code used in his antenna analyser project, for controlling the AD9850 DDS.

#ifndef _DDS_H_
#define _DDS_H_

#include "PinAssignments.h"

typedef long Hz;

class DDS {
public:
    
    void resetDDS() {
        digitalWrite(ddsResetOut, HIGH);
    
        // tRS = 5 clock cycle reset width:
        // (5/125,000,000) secs in milliseconds = 4 ms
        delay(4);
    
        digitalWrite(ddsResetOut, LOW);
    }

    // This used to be a double...
    void setDDSFrequency(Hz Hz) {
        // Calculate the DDS word - from AD9850 Datasheet
        int32_t f = Hz * ddsWordMultiplier;
    
        // Send one byte at a time
        for (int b=0; b<4; b++, f>>=8){
            sendDDSByte(f & 0xFF);
        }
    
        // 5th byte needs to be zeros
        sendDDSByte(0);
        // Strobe the Update pin to tell DDS to use values
        digitalWrite(ddsUpdateOut, HIGH);
    
        // 7.0ns
        delay(1);
        digitalWrite(ddsUpdateOut, LOW);
    }
    
private:
    static const int32_t ddsWordMultiplier = 4294967295/125000000;
    
    // Bit bang the byte over the SPI bus 
    void sendDDSByte(byte dataToSend) { 
        for (int i=0; i<8; i++, dataToSend>>=1){
            // Set Data bit on output pin 
            digitalWrite(ddsDataOut, dataToSend & 0x01);
        
            // Strobe the clock pin 
            digitalWrite(ddsClockOut, HIGH);
            digitalWrite(ddsClockOut, LOW);
        }
    }
};


#endif
