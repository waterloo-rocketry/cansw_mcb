
#ifndef PWM_H
#define	PWM_H

#include "timer.h"
#include <xc.h>
#include "stdint.h"

#define _XTAL_FREQ 12000000 //12MHz
#define MAX_CANARD_ANGLE 10 //+- 10 degrees

void pwm_init(void);

void updatePulseWidth(uint8_t angle);

#endif	/* PWM_H */

