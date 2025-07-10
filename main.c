#include "can_handler.h"
#include "config.h"
#include "pwm.h"
#include "setup.h"
#include "stdbool.h"
#include "stdint.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#define _XTAL_FREQ 12000000 // 12 MHz

#if (BOARD_INST_UNIQUE_ID == PRIMARY)
volatile uint16_t cmd_angle;
volatile bool new_cmd = 0;

#elif (BOARD_INST_UNIQUE_ID == FAILSAFE)
#include "potentiometer.h"
#include "current_sensor.h"
volatile uint16_t adc_value; // potentiometer reading directly from adc
volatile bool new_adc = 0;
uint16_t test;
float current;
float voltage;
uint16_t angle;
w_status_t success;
#endif

static void __interrupt() ISR(void) {
    // Handle CAN interrupts
    if (PIR5) {
        can_handle_interrupt();
    }
    // Handle Timer0 Overflow Interrupt
    if (PIR3bits.TMR0IF) {
        timer0_handle_interrupt(); // Update millis()
        PIR3bits.TMR0IF = 0; // Clear Timer0 interrupt flag
    }

#if (BOARD_INST_UNIQUE_ID == FAILSAFE)
    // Handle ADC interrupt for potentiometer
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0; // Clear ADC interrupt flag

        // Read ADC result
        adc_value = (uint16_t)((ADRESH << 8) + ADRESL);
        new_adc = 1;
    }
#endif
}

int main(void) {
    pin_init();
    osc_init();
    timer0_init();
    pwm_init();
    can_setup();
#if (BOARD_INST_UNIQUE_ID == FAILSAFE)
    pot_init();
    i2c_pin_init();
    i2c_init(0b011);
    current_sense_init();
    // 10s delay to allow servo to move to zero
    for(uint16_t i = 0; i < 10000; i++) {
        __delay_ms(1);
        CLRWDT(); // clear the watchdog timer
    }
    const uint16_t pot_zero_reading = pot_zero();
    uint32_t last_sensor_measure_millis = 0;
    uint32_t last_pot_send_millis = 0;
#endif

    // Enable global interrupts
    INTCON0bits.GIE = 1;

    // loop timer
    uint32_t last_millis = 0;

    while (1) {
        CLRWDT();

        if (OSCCON2 != 0x70) { // If the fail-safe clock monitor has triggered
            osc_init();
        }

        if ((millis() - last_millis) >= MAX_LOOP_TIME_DIFF_ms) {
            last_millis = millis();
            HEARTBEAT();
            LATA1 = 1;

            send_status_ok();
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
            angle = get_angle(filter_potentiometer(adc_value), pot_zero_reading);
        }

        if ((millis() - last_sensor_measure_millis) >= SENSOR_MEASURE_TIME_DIFF_ms) {
            last_sensor_measure_millis = millis();
            pot_read(0x02);
            current = current_read();
            current = current_read();
            voltage = voltage_read();
            voltage = voltage_read();
        }

        if ((millis() - last_pot_send_millis) >= MAX_POT_SEND_TIME_DIFF_ms) {
            last_pot_send_millis = millis();
            can_msg_t angle_msg;

            build_analog_data_msg(
                PRIO_HIGHEST, millis(), SENSOR_CANARD_ENCODER_1, angle, &angle_msg
            );
            txb_enqueue(&angle_msg);
        }
#endif
        txb_heartbeat();
    }
}
