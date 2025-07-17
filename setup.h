#ifndef SETUP_H
#define SETUP_H

#include "message_types.h"
#include "stdbool.h"
#include <xc.h>
#define PRIMARY 0x04
#define FAILSAFE 0x05

// Time between main loop code execution
#define MAX_LOOP_TIME_DIFF_ms 1000
// Time between sending updated angle measurements
#define POT_SEND_TIME_DIFF_ms 5
// Time between potentiometer measurements
#define POT_MEASURE_TIME_DIFF_ms 1
// Return servo to zero if lack of commands exceeds this time
#define NO_CMD_TIME_DIFF_ms 100
// Current measurement for logging
#define CURR_MEASURE_TIME_DIFF_ms 200

#define HEARTBEAT() (LATA0 = !LATA0)

void pin_init(void);

void osc_init(void);

#endif /* SETUP_H */
