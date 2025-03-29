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

void __interrupt() ISR(void) {
    if (PIR5) {
        can_handle_interrupt();
    }
    // Handle Timer0 Overflow Interrupt
    if (PIR3bits.TMR0IF) {
        timer0_handle_interrupt();  // Update millis()
        PIR3bits.TMR0IF = 0;  // Clear Timer0 interrupt flag
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