#include "potentiometer.h"

const int zero_reading = 2048;

void pot_init(void) {
    // Set AN2 as analog input
    TRISA2 = 1; // Set RA2 as input
    ANSELA2 = 1; // Enable analog function on RA2

    // Configure ADC
    ADCON0bits.ON = 1; // enable ADC
    ADCLK = 0b000111; // Fosc/16
    ADCON0bits.CS = 0; // Clock supplied by FOSC, divided according to ADCLK register

    ADCON0bits.FM = 1; // right justified

    FVRCONbits.FVREN = 1; // enable FVR
    FVRCONbits.ADFVR = 0b11; // FVR Buffer 2 Gain is 4x (4.096V)

    while (!FVRCONbits.FVRRDY) {}

    ADREFbits.NREF = 0; // Vref- connected to Vss
    ADREFbits.PREF = 0b11; // Vref+ connected to FVR

    // Enable ADC Interrupts
    PIE1bits.ADIE = 1; // Enable ADC interrupt
    PIR1bits.ADIF = 0; // Clear ADC interrupt flag
}

void pot_read(uint8_t channel) {
    // Select ADC Channel
    ADPCH = channel; // which potentiometer to read

    // Start ADC conversion
    ADCON0bits.GO = 1;
}

uint16_t get_angle(int adc_value) {
    return (adc_value - zero_reading) * ADC_ANGLE_CONVERSION_FACTOR_mdeg + 32768;
}

uint16_t filter_potentiometer(uint16_t new_reading) {
    const float alpha = 0.2;
    static uint16_t filtered_value = 0;

    if (filtered_value == 0) {
        filtered_value = new_reading;
    }

    filtered_value = alpha * new_reading + (1 - alpha) * filtered_value;
    return filtered_value;
}
