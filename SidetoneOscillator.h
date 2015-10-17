// EasiBuild Mk 2 Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//
// Acknowledgements:
//
// Sidetone generation using Martin Nawrath's DDS sine wave generator:
//  http://interface.khm.de/index.php/lab/interfaces-advanced/arduino-dds-sinewave-generator/

#ifndef _SIDETONE_OSCILLATOR_H_
#define _SIDETONE_OSCILLATOR_H_

#include "PinAssignments.h"

//==============================================================================
//=== SIDETONE OSCILLATOR (Thanks to Martin Nawrath)
//==============================================================================

void shapeSidetone(); // forward declaration

// table of 256 sine values / one sine period / stored in flash memory
const uint8_t sine256[] PROGMEM = {
  127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,233,234,236,238,239,240,
  242,243,244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,244,243,242,240,239,238,236,234,233,231,229,227,225,223,
  221,219,217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,78,
  76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,
  33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124

};
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

double dfreq;
// const double refclk=31372.549;  // =16MHz / 510
const double refclk=31376.6;      // measured

// variables used inside interrupt service declared as voilatile
volatile byte icnt;              // var inside interrupt
volatile byte icnt1;             // var inside interrupt
volatile byte c4ms;              // counter incremented all 4ms
volatile unsigned long phaccu;   // phase accumulator
volatile unsigned long tword_m;  // dds tuning word m
volatile double volume;
//volatile int16_t volumeIncrement;

void setSidetoneFrequency(const double dfreq) {
  cbi(TIMSK3, TOIE3);                  // disble Timer3 Interrupt
  tword_m=pow(2,32) * dfreq / refclk;  // calulate DDS new tuning word
  sbi(TIMSK3, TOIE3);                  // enable Timer3 Interrupt 
  /*
  Serial.print(dfreq);
  Serial.print("  ");
  Serial.println(tword_m);
  */
}

//******************************************************************
// Sidetone Timer3 setup
// set prescaler to 1, PWM mode to phase correct PWM,  16000000/510 = 31372.55 Hz clock
void setupSidetoneTimer3() {
  volume = 0.0;
  pinMode(sidetoneOut, OUTPUT);

  // Timer3 Clock Prescaler to : 1 (no prescaling; use system clock)
  sbi (TCCR3B, CS30);
  cbi (TCCR3B, CS31);
  cbi (TCCR3B, CS32);

  // Timer3 PWM Mode set to Phase Correct PWM
  cbi (TCCR3A, COM3A0);  // clear OC3A on Compare Match when counting up, set on Compare Match when counting down
  sbi (TCCR3A, COM3A1);

  sbi (TCCR3A, WGM30);  // Mode 1  / Phase Correct PWM
  cbi (TCCR3A, WGM31);
  cbi (TCCR3B, WGM32);
  
  setSidetoneFrequency(500.0);
  
  sbi (TIMSK3,TOIE3);              // enable Timer3 Interrupt  
}

//******************************************************************
// Timer3 Interrupt Service at 31372,550 KHz = 32uSec
// this is the timebase REFCLOCK for the DDS generator
// FOUT = (M (REFCLK)) / (2 exp 32)
ISR(TIMER3_OVF_vect) {

  phaccu = phaccu + tword_m; // soft DDS, phase accu with 32 bits
  icnt = phaccu >> 24;     // use upper 8 bits for phase accu as frequency information
                           // read value fron ROM sine table and send to PWM DAC
  OCR3A = (byte) ((double) (pgm_read_byte_near(sine256 + icnt) * volume));
  
  if(icnt1++ == 125) {  // increment variable c4ms all 4 milliseconds
    c4ms++;
    icnt1=0;
  }
  
  shapeSidetone();
}


//==============================================================================
//=== SIDETONE SHAPING
//==============================================================================
volatile bool transmit = false; // true when 'morse key' down
volatile int8_t txState = 0;
#define TXS_IDLE_SILENT 0
#define TXS_ENABLE_RAMPUP 1
#define TXS_ON 2
#define TXS_DISABLE_RAMPDOWN 3
volatile int8_t rampCycles = 0;
#define RAMP_CYCLES 5

// called at end of sidetone PWM timer
void shapeSidetone() {
    switch (txState) {
        case TXS_IDLE_SILENT:
            if (transmit) {
                rampCycles = 0;
                txState = TXS_ENABLE_RAMPUP;
                volume = 0.0;
            }
            break;
        case TXS_ENABLE_RAMPUP:
            if (!transmit) { // early termination, bounce?
                txState = TXS_DISABLE_RAMPDOWN;
            } else {
                if (rampCycles == RAMP_CYCLES) {
                    txState = TXS_ON;
                    volume = 1.0;
                } else {
                    volume = rampCycles/RAMP_CYCLES;
                    rampCycles++;
                }
            }
            break;
        case TXS_ON:
            if (!transmit) { // key up?
                txState = TXS_DISABLE_RAMPDOWN;
                rampCycles = RAMP_CYCLES;
            }
            break;
        case TXS_DISABLE_RAMPDOWN:
            if (transmit) { // key down again
                rampCycles = 0;
                txState = TXS_ENABLE_RAMPUP;
            } else {
                if (rampCycles == 0) {
                    txState = TXS_IDLE_SILENT;
                    volume = 0;
                } else {
                    volume = rampCycles/RAMP_CYCLES;
                    rampCycles--;
                }
            }
            break;
    }
}

#endif


