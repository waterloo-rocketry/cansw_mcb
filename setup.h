#ifndef MCB_H
#define	MCB_H


#include <xc.h>
#include "stdbool.h"

// Time between main loop code execution
#define MAX_LOOP_TIME_DIFF_ms 500
// Time between "high speed" sensor checks
#define MAX_SENSOR_LOOP_TIME_DIFF_ms 5

#define HEARTBEAT() (LATA0 = !LATA0)

void pin_init(void);

void osc_init(void);

#endif	/* MCB_H */
