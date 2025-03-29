#include <xc.h>
#include "stdint.h"
#include "stdbool.h"     
#include "setup.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include "pwm.h"
#include "can_handler.h"

#include "potentiometer.h"

#define _XTAL_FREQ 12000000 //12MHz

volatile uint16_t adc_value; //potentiometer reading directly from adc

static void __interrupt() ISR(void) {
    //Handle CAN interrupts
    if (PIR5) {
        //can_handle_interrupt();
    }
    // Handle Timer0 Overflow Interrupt
    if (PIR3bits.TMR0IF) {
        timer0_handle_interrupt();  // Update millis()
        PIR3bits.TMR0IF = 0;  // Clear Timer0 interrupt flag
    }
    
    //Handle ADC interrupt
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;  // Clear ADC interrupt flag

        // Read ADC result
        adc_value = ((uint16_t)ADRESH << 8) | ADRESL;
    }
}

int main(void) {
    pin_init();
    osc_init();
    //LATA1 = 0;
    timer0_init();
    pwm_init();
    can_setup();
    // Enable global interrupts
    INTCON0bits.GIE = 1;
    
    // loop timer
    uint32_t last_millis = 0;
    
    while(1) {
        CLRWDT();
        
        if ((millis() - last_millis) > MAX_LOOP_TIME_DIFF_ms) {
            last_millis = millis();
            HEARTBEAT();
        }
        
        updatePulseWidth(100);
    }
    
}