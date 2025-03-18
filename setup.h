#ifndef MCB_H
#define	MCB_H

#define MCU 0
#define FS 1

#define BOARD_ID MCU
#ifndef BOARD_ID
#define BOARD_ID FS
#endif


#include <xc.h>
#include "stdbool.h"

// Time between main loop code execution
#define MAX_LOOP_TIME_DIFF_ms 500
// Time between "high speed" sensor checks
#define MAX_SENSOR_LOOP_TIME_DIFF_ms 5

// LEDs
#if (BOARD_ID == MCU)
#define RED_LED_ON() (LATA0 = 0)
#define RED_LED_OFF() (LATA0 = 1)
#define GREEN_LED_ON() (LATA1 = 0)
#define GREEN_LED_OFF() (LATA1 = 1)

#elif (Board_ID == FS)
#define WHITE_LED_ON() (LATA0 = 0)
#define WHITE_LED_OFF() (LATA1 = 1)
#define BLUE_LED_ON() (LATA0 = 0)
#define BLUE_LED_OFF() (LATA1 = 1)
#endif

void pin_init(void);

void osc_init(void);

void heartbeat(bool last_heartbeat);


#endif	/* MCB_H */
