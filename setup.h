#ifndef SETUP_H
#define SETUP_H

#include "message_types.h"
#include "stdbool.h"
#include <xc.h>
#define PRIMARY 0x02
#define FAILSAFE 0x03

// Time between main loop code execution
#define MAX_LOOP_TIME_DIFF_ms 1000
// Time between sending updated angle measurements
#define MAX_POT_SEND_TIME_DIFF_ms 5
// Time between potentiometer measurements
#define SENSOR_MEASURE_TIME_DIFF_ms 500

#define HEARTBEAT() (LATA0 = !LATA0)

void pin_init(void);

void osc_init(void);

#endif /* SETUP_H */
