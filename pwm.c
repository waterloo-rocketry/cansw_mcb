#include "pwm.h"

const uint16_t MOTOR_MIN_PULSE_WIDTH_US = 500; // corresponds to -10 degrees
const uint16_t MOTOR_MAX_PULSE_WIDTH_US = 2500; // corresponds to 10 degrees
const uint16_t MOTOR_CENTER_PULSE_WIDTH_US = 1500; // corresponds to 0 degrees

// Motor control functions
void pwm_init(void) {
    // 300 Hz PWM

    // 1. Use the desired output pin RxyPPS control to select CCPx as the source and
    //    disable the CCPx pin output driver by setting the associated TRIS bit.

#if (BOARD_INST_UNIQUE_ID == PRIMARY)
    RB3PPS = 0b001011;
    TRISB3 = 1;

#elif (BOARD_INST_UNIQUE_ID == FAILSAFE)
    RB4PPS = 0b001011;
    TRISB4 = 1;
#endif

    // 2. Load the T2PR register with the PWM period value.
    T2PR = 155;

    // 3. Configure the CCP module for the PWM mode by loading the CCPxCON register with the
    // appropriate values.
    CCP3CONbits.EN = 0b1; // enable CCP3
    CCP3CONbits.FMT = 0b0; // Set to right-aligned
    CCP3CONbits.MODE = 0b1100; // set to PWM operation

    // 4. Load the CCPRxL register, and the CCPRxH register with the PWM duty cycle value and
    // configure the FMT bit of the CCPxCON register to set the proper register alignment.
#if (BOARD_INST_UNIQUE_ID == PRIMARY)
    CCPR3H = (562 >> 8) & 0xFF; // upper 8 bits corresponding to 1500uS
    CCPR3L = 562 & 0xFF; // lower 8 bits corresponding to 1500uS
#endif
    // 5. Configure and start Timer2:
    //- Clear the TMR2IF interrupt flag bit of the respective PIR register. See Note below.
    PIR4bits.TMR2IF = 0;
    //- Select the timer clock source to be as FOSC/4 using the T2CLK register.
    T2CLK = 0b0001; //(pg 321)
    //- Configure the CKPS bits of the T2CON register with the Timer prescale value.
    T2CONbits.CKPS = 0b110; // prescale of 64
    //- Enable the Timer by setting the ON bit of the T2CON register.
    T2CONbits.ON = 1; // enables timer

    // 6. Enable PWM output pin:
    //- Wait until the Timer overflows and the TMR2IF bit of the PIR4 register is set. See Note
    while (PIR4bits.TMR2IF == 0) {}

    //- Enable the CCPx pin output driver by clearing the associated TRIS bit.
#if (BOARD_INST_UNIQUE_ID == PRIMARY)
    TRISB3 = 0;

    // Disable the CCPx pin output driver for failsafe to not interfere with PWM signal. Will be
    // enabled when failsafe overrides
#elif (BOARD_INST_UNIQUE_ID == FAILSAFE)
    TRISB4 = 1;
#endif
}

void updatePulseWidth(uint16_t angle) {
    float normalized_angle =
        (float)angle - (32768 - MAX_CANARD_ANGLE_MDEG); // convert 32768 +- 10000 to 0-20000
    float pulse_width_us = MOTOR_MIN_PULSE_WIDTH_US +
                           (normalized_angle * (MOTOR_MAX_PULSE_WIDTH_US - MOTOR_MIN_PULSE_WIDTH_US)
                           ) / (MAX_CANARD_ANGLE_MDEG * 2);
    uint8_t tmr2_prescale = 1 << T2CONbits.CKPS;
    uint16_t bitWrite = (uint16_t)(pulse_width_us / 1000000 * _XTAL_FREQ / tmr2_prescale);
    CCPR3H = (bitWrite >> 8) & 0xFF;
    CCPR3L = bitWrite & 0xFF;
}

