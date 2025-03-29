#include "potentiometer.h"

void pot_init(void) {
    // Set AN0 as analog input
    TRISA2 = 1;  // Set RA0 as input
    ANSELA2 = 1; // Enable analog function on RA0
    
    
    // Configure ADC
    ADCON0bits.ADON = 1;  // enable ADC
    ADCLK = 0b000111; //Fosc/16
    ADCON0bits.ADCS = 0; // Clock supplied by FOSC, divided according to ADCLK register
    ADCON0bits.ADFM = 1; //right justified
    
    //Enable ADC Interrupts
    PIE1bits.ADIE = 1;   // Enable ADC interrupt
    PIR1bits.ADIF = 0;   // Clear ADC interrupt flag
}

void pot_read(uint8_t channel) {
    //Select ADC Channel
    ADPCH = channel; //which potentiometer to read
    
    //Start ADC conversion
    ADCON0bits.ADGO = 1;
}
