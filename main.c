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
volatile bool new_cmd = 0;

#elif (BOARD_INST_UNIQUE_ID == FAILSAFE)
#include "potentiometer.h"
volatile uint16_t adc_value; //potentiometer reading directly from adc
volatile bool new_adc = 0;

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
    
#if (BOARD_INST_UNIQUE_ID == FAILSAFE)
    //Handle ADC interrupt for potentiometer
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;  // Clear ADC interrupt flag

        // Read ADC result
        adc_value = ((uint16_t)ADRESH << 8) | ADRESL;
        new_adc = 1;
        
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
#if (BOARD_INST_UNIQUE_ID == FAILSAFE)
    pot_init();
    uint16_t current_angle;
    uint32_t last_pot_measure_millis = 0;
    uint32_t last_pot_send_millis = 0;
#endif
    
    // Enable global interrupts
    INTCON0bits.GIE = 1;
    
    // loop timer
    uint32_t last_millis = 0;
    
    while(1) {
        CLRWDT();
 
        
        if (OSCCON2 != 0x70) { // If the fail-safe clock monitor has triggered
            osc_init();
        }
        
        if ((millis() - last_millis) > MAX_LOOP_TIME_DIFF_ms) {
            last_millis = millis();
            HEARTBEAT();
            LATA1 = 1;
            
        }
                
#if (BOARD_INST_UNIQUE_ID == PRIMARY)
        if (new_cmd) {
            updatePulseWidth(cmd_angle);
            new_cmd = 0;
            LATA1 = 0;
        }
#elif (BOARD_INST_UNIQUE_ID == FAILSAFE)
        if (new_adc) {
            new_adc = 0;
            current_angle = adc_value;
            
        }
        
        if ((millis() - last_pot_measure_millis) > MAX_POT_MEASURE_TIME_DIFF_ms) {
            pot_read(0x03);
            last_pot_measure_millis = millis();
        }
        
        if ((millis() - last_pot_send_millis) > MAX_POT_SEND_TIME_DIFF_ms) {
            can_msg_t angle_msg;
            
            build_analog_data_msg(PRIO_HIGHEST, millis(), SENSOR_CANARD_ENCODER_1, current_angle, &angle_msg);
            txb_enqueue(&angle_msg);
            last_pot_send_millis = millis();
        }
#endif
        txb_heartbeat();
    }
}