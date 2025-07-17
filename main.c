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
uint16_t current;
uint16_t voltage;
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
#if (BOARD_INST_UNIQUE_ID == PRIMARY)
    uint32_t last_cmd_millis = 0;
#elif (BOARD_INST_UNIQUE_ID == FAILSAFE)
    pot_init();
    i2c_pin_init();
    i2c_init(0b011);
    current_sense_init();
    LATA1 = 0; //indicate zeroing servo
    // 10s delay to allow servo to move to zero
    for(uint16_t i = 0; i < 10000; i++) {
        __delay_ms(1);
        CLRWDT(); // clear the watchdog timer
    }
    const uint16_t pot_zero_reading = pot_zero();
    LATA1 = 1; //indicate servo zero complete
    uint32_t last_pot_measure_millis = 0;
    uint32_t last_pot_send_millis = 0;
    uint32_t last_curr_measure_millis = 0;
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
            uint16_t gen_err_bitfield = 0;
            
#if (BOARD_INST_UNIQUE_ID == FAILSAFE)
            voltage = voltage_read();
            voltage_read(); // dummy read because i2c driver fails every other call
            can_msg_t voltage_msg;
            build_analog_data_msg(PRIO_LOW, millis(), SENSOR_BATT_VOLT, voltage, &voltage_msg);
            txb_enqueue(&voltage_msg);
            
            if (voltage <= UNDERVOLTAGE_THRESHOLD_BATT_mV) {
                gen_err_bitfield |= (1 << E_BATT_UNDER_VOLTAGE_OFFSET);
            }
            
            else if (voltage >= OVERVOLTAGE_THRESHOLD_BATT_mV) {
                gen_err_bitfield |= (1 << E_BATT_OVER_VOLTAGE_OFFSET);
            }
            
            if (current >= OVERCURRENT_THRESHOLD_BATT_mA) {
                gen_err_bitfield |= (1 << E_BATT_OVER_CURRENT_OFFSET);
            }
            
#endif
            can_msg_t board_stat_msg;
            build_general_board_status_msg(PRIO_LOW, millis(), gen_err_bitfield, 0, &board_stat_msg);
            txb_enqueue(&board_stat_msg);
        }

#if (BOARD_INST_UNIQUE_ID == PRIMARY)
        if (new_cmd) {
            updatePulseWidth(cmd_angle);
            new_cmd = 0;
            last_cmd_millis = millis();
            LATA1 = 1; //clear error LED
        }
        
        if ((millis() - last_cmd_millis) >= NO_CMD_TIME_DIFF_ms) {
            updatePulseWidth(32768); //lock to zero if no cmd from proc
            LATA1 = 0; //error indication LED
        }
#elif (BOARD_INST_UNIQUE_ID == FAILSAFE)
        if (new_adc) {
            new_adc = 0;
            angle = get_angle(filter_potentiometer(adc_value), pot_zero_reading);
        }

        if ((millis() - last_pot_measure_millis) >= POT_MEASURE_TIME_DIFF_ms) {
            last_pot_measure_millis = millis();
            pot_read(0x02);
        }

        if ((millis() - last_pot_send_millis) >= POT_SEND_TIME_DIFF_ms) {
            last_pot_send_millis = millis();
            can_msg_t angle_msg;

            build_analog_data_msg(PRIO_HIGHEST, millis(), SENSOR_CANARD_ENCODER_1, angle, &angle_msg);
            txb_enqueue(&angle_msg);
        }
        
        if ((millis() - last_curr_measure_millis) >= CURR_MEASURE_TIME_DIFF_ms) {
            last_curr_measure_millis = millis();
            
            current = current_read();
            current_read(); // dummy read because i2c driver fails every other call
            
            can_msg_t curr_msg;
            build_analog_data_msg(PRIO_LOW, millis(), SENSOR_BATT_CURR, current, &curr_msg);
            txb_enqueue(&curr_msg);
        }
#endif
        txb_heartbeat();
    }
}
