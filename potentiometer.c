#include "potentiometer.h"

void pot_init(void) {
    // Set AN3 as analog input
    TRISA2 = 1;  // Set RA2 as input
    ANSELA2 = 1; // Enable analog function on RA2
    
    // Configure ADC
    ADCON0bits.ON = 1;  // enable ADC
    ADCLK = 0b000111; //Fosc/16
    ADCON0bits.CS = 0; // Clock supplied by FOSC, divided according to ADCLK register
   
    ADCON0bits.FM = 1; //right justified
    
    FVRCONbits.FVREN = 1; //enable FVR
    FVRCONbits.ADFVR = 0b11; // FVR Buffer 2 Gain is 4x (4.096V)
            
    //Enable ADC Interrupts
    PIE1bits.ADIE = 1;   // Enable ADC interrupt
    PIR1bits.ADIF = 0;   // Clear ADC interrupt flag
}

void pot_read(uint8_t channel) {
    //Select ADC Channel
    ADPCH = channel; //which potentiometer to read
    
    //Start ADC conversion
    ADCON0bits.GO = 1;
}

uint16_t get_current_angle(uint16_t adc_value) {
    
}

uint16_t get_filtered_angle(uint16_t current_angle, uint16_t prev_angle) {
    return SMOOTHING_FACTOR * current_angle + (1-SMOOTHING_FACTOR) * prev_angle; //exponential low pass filter
}
