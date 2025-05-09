
#ifndef PWM_H
#define PWM_H

#include "setup.h"
#include "stdint.h"
#include "timer.h"
#include <xc.h>

#define _XTAL_FREQ 12000000 // 12 MHz
#define MAX_CANARD_ANGLE_MDEG 10000 //+- 10 degrees

void pwm_init(void);

void updatePulseWidth(uint16_t angle);

#endif /* PWM_H */

