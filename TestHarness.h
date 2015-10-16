// EasiBuild Mk 2 Test Harness Firmware
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _TEST_HARNESS_H_
#define _TEST_HARNESS_H_

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
    setupPins();
    setupDisplay();
    
    setupSidetoneTimer3();

            // 0123456789ABCDEF
    lcd.print(".LRB????       ");
    Serial.begin(115200);
    
    mySCoop.start();
}

// main loop
//bool oldtransmit = true; // display fostamin    
//char ch = 'x';
//int oldstate = 99; // display fostamin
uint16_t oldPinB = 0xffff; // display fostamin
uint16_t newPinB = 0;
uint16_t bit;
int bufidx;
char buffer[20];
int countx = 0;
void loop() {
    yield();

/*
    if (digitalRead(dit_in) == LOW) {
        transmit = true;
    }
    if (digitalRead(dah_in) == LOW) {
        transmit = false;
    }

    if (oldtransmit != transmit) {
        lcd.setCursor(0, 1);
        lcd.print(transmit ? "*" : "O");
        oldtransmit = transmit;
    }
    
    if (oldstate != txState) {
        switch (txState) {
            case TXS_IDLE_SILENT:
                ch = 'i';
                break;
            case TXS_ENABLE_RAMPUP:
                ch = 'u';
                break;
            case TXS_ON:
                ch = 'o';
                break;
            case TXS_DISABLE_RAMPDOWN:
                ch = 'd';
                break;
        }
        lcd.setCursor(1, 1);
        lcd.print(ch);
        lcd.print(' ');
        lcd.print(rampCycles);
        lcd.print(' ');
        lcd.print(volume);
        lcd.print(' ');
        oldstate = txState;
    }
    */
    newPinB = PINB;
    if (oldPinB != newPinB) {
       lcd.setCursor(9, 0);
       sprintf(buffer, "0x%04X", countx++);
       lcd.print(buffer);
       
       lcd.setCursor(0, 1);
       bit = 0x0080;
       bufidx = 0;
       while (bit != 0) {
           buffer[bufidx++] = (newPinB & bit) == 0 ? '0' : '1';
           bit >>= 1;
       }
       
       sprintf(buffer + bufidx, " 0x%04X", newPinB);
       lcd.print(buffer);
       oldPinB = newPinB;
    }
}


#endif

