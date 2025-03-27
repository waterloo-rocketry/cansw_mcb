#ifndef MCB_H
#define	MCB_H


#include <xc.h>
#include "stdbool.h"

// Time between main loop code execution
#define MAX_LOOP_TIME_DIFF_ms 500
// Time between "high speed" sensor checks
#define MAX_SENSOR_LOOP_TIME_DIFF_ms 5

#define HEARTBEAT() (LATA0 = !LATA0)

// LEDs
#if (BOARD_INST_UNIQUE_ID == BOARD_INST_ID_CANARD_MOTOR_PRIMARY)
#define RED_LED_ON() (LATA0 = 0)
#define RED_LED_OFF() (LATA0 = 1)
#define GREEN_LED_ON() (LATA1 = 0)
#define GREEN_LED_OFF() (LATA1 = 1)

#elif (BOARD_INST_UNIQUE_ID == BOARD_INST_ID_CANARD_MOTOR_FAILSAFE)
#define WHITE_LED_ON() (LATA0 = 0)
#define WHITE_LED_OFF() (LATA1 = 1)
#define BLUE_LED_ON() (LATA0 = 0)
#define BLUE_LED_OFF() (LATA1 = 1)
#endif

void pin_init(void);

void osc_init(void);

#endif	/* MCB_H */
