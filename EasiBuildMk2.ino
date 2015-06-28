// EasiBuild Mk 2 Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>
//
// For Arduino Micro. If you want to target a different board, search for the comments 'CHANGE_BOARD';
// if you want to change the pins used for the various inputs/outputs, search for the comments 'CHANGE_PIN'.
//
//
// Acknowledgements:
// SCoop: Fabrice Oudert http://forum.arduino.cc/index.php?topic=137801.0
//
// LiquidCrystal_I2C: Francisco Malpartida: https://arduino-info.wikispaces.com/LCD-Blue-I2C
//
// Sidetone generation using Adrian Freed's DDS sine wave generator:
//   http://www.adrianfreed.com/content/arduino-sketch-high-frequency-precision-sine-wave-tone-sound-synthesis

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// CHANGE_BOARD
#include <Arduino.h> 

#include <SCoop.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// CHANGE_PINS
const int dah_in = 12;
const int dit_in = 11;
const int encr_in = 10;
const int encl_in = 9;
const int btn_in = 8;
const int band1_out = 7;
const int band2_out = 19;
const int band3_out = 18;
const int dds_clock_out = 14;
const int dds_update_out = 15;
const int dds_data_out = 16;
const int dds_reset_out = 17;
const int sidetone_out = 6;
const int rxtx_out = 5;
const int key_out = 4;
// display is on 2 and 3, i2c.

//==============================================================================
//=== SIDETONE OSCILLATOR (Adrian Freed)
//==============================================================================

// Atmega table-based digital oscillator
// using "DDS" with 32-bit phase register to illustrate efficient
// accurate frequency.
// 20-bits is on the edge of people pitch perception
// 24-bits has been the usual resolution employed.
// so we use 32-bits in C, i.e. long.
//
// smoothly interpolates frequency and amplitudes illustrating
// lock-free approach to synchronizing foreground process control and
// background (interrupt) sound synthesis.
// copyright 2009. Adrian Freed. All Rights Reserved.
// Use this as you will but include attribution in derivative works.
// tested on the Arduino Mega
const unsigned int LUTsize = 1<<8;
// Look Up Table size: has to be power of 2 so that the modulo LUTsize
// can be done by picking bits from the phase
// avoiding arithmetic
const int8_t sintable[LUTsize] PROGMEM = { // already biased with +127
    127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,
    176,179,182,184,187,190,193,195,198,200,203,205,208,210,213,215,
    217,219,221,224,226,228,229,231,233,235,236,238,239,241,242,244,
    245,246,247,248,249,250,251,251,252,253,253,254,254,254,254,254,
    255,254,254,254,254,254,253,253,252,251,251,250,249,248,247,246,
    245,244,242,241,239,238,236,235,233,231,229,228,226,224,221,219,
    217,215,213,210,208,205,203,200,198,195,193,190,187,184,182,179,
    176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,
    127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,
    78,75,72,70,67,64,61,59,56,54,51,49,46,44,41,39,
    37,35,33,30,28,26,25,23,21,19,18,16,15,13,12,10,
    9,8,7,6,5,4,3,3,2,1,1,0,0,0,0,0,
    0,0,0,0,0,0,1,1,2,3,3,4,5,6,7,8,
    9,10,12,13,15,16,18,19,21,23,25,26,28,30,33,35,
    37,39,41,44,46,49,51,54,56,59,61,64,67,70,72,75,
    78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124
};

const int timerPrescale=1<<9;

struct oscillator
{
    uint32_t phase;
    int32_t phase_increment;
    int32_t frequency_increment;
    int16_t amplitude;
    int16_t amplitude_increment;
    uint32_t framecounter;
} o1;

const int fractionalbits = 16; // 16 bits fractional phase
// compute a phase increment from a frequency
unsigned long phaseinc(float frequency_in_Hz)
{
    return LUTsize *(1l<<fractionalbits)* frequency_in_Hz/(F_CPU/timerPrescale);
}

// Timer setup constants
// CHANGE_BOARD
// CHANGE_PINS
#define PWM_VALUE_DESTINATION OCR1B
#define PWM_INTERRUPT TIMER1_OVF_vect


void initializeOscillatorTimer() {
    // Set up PWM with Clock/256 (i.e. 31.25kHz on Arduino 16MHz;
    // and phase accurate
    // Timer 1
    TCCR1A = _BV(COM1B1) | _BV(WGM10);
    TCCR1B = _BV(CS10);
    TIMSK1 = _BV(TOIE1);
    pinMode(sidetone_out, OUTPUT);
}

void shapeSidetone(); // forward declaration

// this is the heart of the wavetable synthesis. A phasor looks up a sine table
int8_t outputvalue = 0;
SIGNAL(PWM_INTERRUPT)
{
    PWM_VALUE_DESTINATION = outputvalue; // output first to minimize jitter
    outputvalue = (((uint8_t) (o1.amplitude >> 8)) *
       pgm_read_byte(sintable + ((o1.phase >> 16) % LUTsize))) >> 8;
    o1.phase += (uint32_t) o1.phase_increment;

    shapeSidetone();

    // ramp amplitude and frequency
    // TODO not needed?
    if (o1.framecounter > 0)
    {
        --o1.framecounter;
        o1.amplitude += o1.amplitude_increment;
        o1.phase_increment += o1.frequency_increment;
    }
}

void setupOscillator()
{
    o1.phase = 0;
    o1.phase_increment = 0 ;
    o1.amplitude_increment = 0; // TODO needed?
    o1.frequency_increment = 0; // TODO needed?
    o1.framecounter = 0; // TODO needed?
    o1.amplitude = 0; // no amplitude

    // TODO get the persistent sidetone frequency from EEPROM
    o1.phase_increment = phaseinc(600.0);

    initializeOscillatorTimer();
}


//==============================================================================
//=== SIDETONE SHAPING
//==============================================================================
volatile bool transmit = false; // true when 'morse key' down
int8_t txState = 0;
#define TXS_IDLE_SILENT 0
#define TXS_ENABLE_RAMPUP 1
#define TXS_ON 2
#define TXS_DISABLE_RAMPDOWN 3
int8_t rampCycles = 0;
#define RAMP_CYCLES 30

// called at end of sidetone PWM timer
void shapeSidetone() {
    switch (txState) {
        case TXS_IDLE_SILENT:
            if (transmit) {
                rampCycles = RAMP_CYCLES;
                txState = TXS_ENABLE_RAMPUP;
                o1.amplitude = 0;
                o1.amplitude_increment = 3; // TODO this could be the sidetone volume?
            }
            break;
        case TXS_ENABLE_RAMPUP:
            if (!transmit) { // early termination, bounce?
                txState = TXS_DISABLE_RAMPDOWN;
                o1.amplitude_increment = -o1.amplitude_increment;
            } else {
                if (rampCycles == 0) {
                    txState = TXS_ON;
                    o1.amplitude_increment = 0; // stop incrementing amplitude
                } else {
                    rampCycles--;
                }
            }
            break;
        case TXS_ON:
            if (!transmit) { // key up?
                txState = TXS_DISABLE_RAMPDOWN;
                rampCycles = RAMP_CYCLES;
                o1.amplitude_increment = -3; // TODO this could be the sidetone volume?
            }
            break;
        case TXS_DISABLE_RAMPDOWN:
            if (transmit) { // key down again
                txState = TXS_ENABLE_RAMPUP;
                o1.amplitude_increment = -o1.amplitude_increment;
            } else {
                if (rampCycles == 0) {
                    txState = TXS_IDLE_SILENT;
                    o1.amplitude = 0;
                    o1.amplitude_increment = 0; // stop decrementing amplitude
                } else {
                    rampCycles--;
                }
            }
            break;
    }
}


//==============================================================================
//=== LCD DISPLAY
//==============================================================================

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//==============================================================================
//=== PIN CHANGE INTERRUPT
//==============================================================================

/*
 *  value = portb & interesting_bits;
 *  if (value == last_portb) {
 *    return;
 *  }
 *
 *  last_portb = value;
 *  this_dah = value & dah;
 *  if (this_dah != last_dah) {
 *    last_dah = this_dah;
 *    raise(dah_event, this_dah ? HIGH : LOW);
 *  }
 *  this_dit = value & dit;
 *  if (this_dit != last_dit) {
 *    last_dit = this_dit;
 *    raise(dit_event, this_dit ? HIGH : LOW);
 *  }
 *  this_btn = value & btn;
 *  if (this_btn != last_btn) {
 *    last_btn = this_btn;
 *    raise(btn_event, this_btn ? HIGH : LOW);
 *  }
 *
 *  // do rotary encoder
 *
 */

//==============================================================================
//=== SCOOP TASKS
//==============================================================================

// user definition of task1 object
defineTask(Task1)
// force any read/write to memory
volatile long count;

void Task1::setup() {
    count = 0;
};

void Task1::loop() {
    sleepSync(1000);
    count++;
};


// user quick definition of task2 object 
defineTaskLoop(Task2)
{   digitalWrite(13, HIGH);
    sleep(100);
    digitalWrite(13, LOW);
    sleep(100);
}

//==============================================================================
//=== MAIN SETUP AND LOOP
//==============================================================================

// main setup
void setup() {
    lcd.begin(16, 2);
    lcd.backlight();
    lcd.setCursor(0, 0);
    
    pinMode(dah_in, INPUT_PULLUP);
    pinMode(dit_in, INPUT_PULLUP);
    pinMode(encr_in, INPUT_PULLUP);
    pinMode(encl_in, INPUT_PULLUP);
    pinMode(btn_in, INPUT_PULLUP);
    
    pinMode(band1_out, OUTPUT);
    pinMode(band2_out, OUTPUT);
    pinMode(band3_out, OUTPUT);
    
    pinMode(rxtx_out, OUTPUT); 
    pinMode(key_out, OUTPUT);
    
    pinMode(dds_clock_out, OUTPUT);
    pinMode(dds_update_out, OUTPUT);
    pinMode(dds_data_out, OUTPUT);
    pinMode(dds_reset_out, OUTPUT);
    
    setupOscillator();

            // 0123456789ABCDEF
    lcd.print("seconds spent :");
    Serial.begin(115200);
    Serial.print("seconds spent :");
    
    mySCoop.start();
}

// main loop
void loop() {
    long oldcount = -1;
    yield();

    transmit = (digitalRead(dit_in) == LOW || digitalRead(dah_in) == LOW); // DIAG

    if (oldcount != count) {
        lcd.setCursor(0, 1);
        lcd.print(count);
        Serial.println(count); 
        oldcount = count;
    }
}

