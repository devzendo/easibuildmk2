// EasiBuild Mk 2 Test Harness Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _SIDETONE_TEST_HARNESS_H_
#define _SIDETONE_TEST_HARNESS_H_

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================

// main setup
void setup() {
    setupPins();
    setupDisplay();

    setupSidetoneTimer3();
}

// main loop
void loop() {

}


#endif

