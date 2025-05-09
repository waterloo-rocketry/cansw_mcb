#include "setup.h"

void pin_init(void) {
    // LEDs
    TRISA0 = 0; // set A0 as an output for the red/white LED
    ANSELA0 = 0; // Enable digital input buffer (Useful for reading the LED state)
    LATA0 = 1; // turn the red/white LED off

    TRISA1 = 0; // set A1 as an output for the green/blue LED
    ANSELA1 = 0; // Enable digital input buffer
    LATA1 = 1; // turn the green/blue LED off

#if (BOARD_INST_UNIQUE_ID == PRIMARY)
    TRISB4 = 1; // Overcurrent alert input
    TRISB5 = 1; // FS active input

#elif (BOARD_INST_UNIQUE_ID == FAILSAFE)
    TRISC5 = 0; // set C5 as output for servo MOSFET
    TRISC5 = 0; // turn servo on (inverted signal)

#endif
}

void osc_init(void) {
    // Use internal oscillator, 12 MHz
    OSCFRQbits.FRQ = 0b0100;
}
