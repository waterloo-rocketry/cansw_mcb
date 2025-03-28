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

static void __interrupt(irq(IRQ_TMR0)) ISR(void) {
    if (PIR3bits.TMR0IF) {  // Check if Timer0 overflowed
        timer0_handle_interrupt();  // Call function to update millis()
        PIR3bits.TMR0IF = 0;  // Clear the interrupt flag
    }
}



int main(void) {
    pin_init();
    osc_init();
    LATA1 = 0;
    timer0_init();
    pwm_init();
    
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