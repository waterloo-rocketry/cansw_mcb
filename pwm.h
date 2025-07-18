
#ifndef PWM_H
#define PWM_H

#include "setup.h"
#include "stdint.h"
#include "timer.h"
#include <xc.h>

#define MAX_CANARD_ANGLE_MDEG 10000 //+- 10 degrees
#define T2_PRESCALE 0b110
#define _XTAL_FREQ_MHZ (_XTAL_FREQ / 1000000)

void pwm_init(void);

void updatePulseWidth(uint16_t angle);

#endif /* PWM_H */

