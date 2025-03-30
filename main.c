#include <xc.h>
#include "stdint.h"
#include "stdbool.h"     
#include "setup.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include "pwm.h"
#include "can_handler.h"

#define _XTAL_FREQ 12000000 //12MHz

#if (BOARD_INST_UNIQUE_ID == PRIMARY)
volatile uint16_t cmd_angle;

#elif (BOARD_INST_UNIQUE_ID == FAILSAFE)
#include "potentiometer.h"
volatile uint16_t adc_value; //potentiometer reading directly from adc

#endif

static void __interrupt() ISR(void) {
    //Handle CAN interrupts
    if (PIR5) {
        can_handle_interrupt();
    }
    // Handle Timer0 Overflow Interrupt
    if (PIR3bits.TMR0IF) {
        timer0_handle_interrupt();  // Update millis()
        PIR3bits.TMR0IF = 0;  // Clear Timer0 interrupt flag
    }
    
#if (BOARD_INST_UNIQUE_ID == 0x03)
    //Handle ADC interrupt for potentiometer
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;  // Clear ADC interrupt flag

        // Read ADC result
        adc_value = ((uint16_t)ADRESH << 8) | ADRESL;
    }
#endif
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
        updatePulseWidth(cmd_angle);
        
    }
    
}