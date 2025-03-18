#include "setup.h"


void pin_init(void){
    // LEDs
    TRISA0 = 0; // set A0 as an output for the red/white LED
    //ANSELA0 = 0; // Enable digital input buffer (Useful for reading the LED state)
    LATA0 = 1; // turn the red/white LED off

    TRISA1 = 0; // set A1 as an output for the green/blue LED
    //ANSELA1 = 0; // Enable digital input buffer
    LATA1 = 1; // turn the green/blue LED off
    
#if (DEVICE_ID == MCU)
    TRISB4 = 1; //Overcurrent alert input
    TRISB5 = 1; //FS active input
    
#elif (DEVICE_ID == FS)
    TRISC5 = 0; // set C5 as output for servo MOSFET
    TRISC5 = 0; //turn servo on (inverted signal)
    
#endif
}

void osc_init(void){
    // Select external oscillator with PLL of 1:1
    OSCCON1 = 0b01110000;
    // wait until the clock switch has happened
    while (OSCCON3bits.ORDY == 0)  {}
    // if the currently active clock (CON2) isn't the selected clock (CON1)
    if (OSCCON2 != 0b01110000) {
        // try to wait for the oscillator to come back
        for (uint16_t i = 0; i < 60000; i++) {}
    }
}

void heartbeat(bool last) {
    LATA0 = !last;
}

#pragma config FEXTOSC = HS     // External Oscillator Selection (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = HFINTOSC_1MHZ// Reset Oscillator Selection (HFINTOSC with HFFRQ = 4 MHz and CDIV = 4:1)

