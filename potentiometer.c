#include "potentiometer.h"

static uint16_t zero_reading;

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
    ADREFbits.PREF = 0b00; // Vref+ connected to Vdd

    // Enable ADC Interrupts
    PIE1bits.ADIE = 1; // Enable ADC interrupt
    PIR1bits.ADIF = 0; // Clear ADC interrupt flag
}

uint16_t pot_zero(void) {
    uint32_t sum_readings = 0;
    for (uint8_t i = 0; i < 100; i++) {
        pot_read(0x02);
        while (ADCON0bits.GO)
            ;
        sum_readings += (uint16_t)((ADRESH << 8) + ADRESL);
        PIR1bits.ADIF = 0; // Clear ADC interrupt flag
        __delay_ms(1);
        CLRWDT(); // Feed the dog
    }
    return sum_readings / 100;
}

void pot_read(uint8_t channel) {
    // Select ADC Channel
    ADPCH = channel; // which potentiometer to read

    // Start ADC conversion
    ADCON0bits.GO = 1;
}

uint16_t get_angle(uint16_t adc_value, uint16_t zero_value) {
    int32_t delta = (int32_t)zero_value - (int32_t)adc_value; // signed difference
    // multiply by 3663 (factor in hundredths of mdeg), then divide by 100
    int32_t mdeg = (delta * ADC_ANGLE_CONVERSION_FACTOR_hund_mdeg) / 100;
    mdeg += 32768; // zero offset
    if (mdeg < 0) {
        mdeg = 0;
    }
    if (mdeg > 0xFFFF) {
        mdeg = 0xFFFF;
    }
    return (uint16_t)mdeg;
}

uint16_t filter_potentiometer(uint16_t new_reading) {
    // a = 1/5, so (1-a)=4/5
    static uint16_t filtered = 0;
    static bool first = true;

    if (first) {
        filtered = new_reading;
        first = false;
    }

    // filtered = a·new + (1-a)·filtered
    // multiply everything by 1, divide by 5:
    filtered = (uint32_t)(new_reading + 4u * filtered + 2u // for rounding: denom/2
               ) /
               5u;

    return filtered;
}
