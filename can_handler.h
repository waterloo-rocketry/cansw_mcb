#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include "canlib.h"
#include "pwm.h"
#include "setup.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

void can_setup(void);

void can_receive_callback(const can_msg_t *msg);

#endif /* CAN_HANDLER_H */

