#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include "canlib.h"
#include "mypwm.h"
#include "setup.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#define _XTAL_FREQ 12000000 // 12 MHz

#define E_NOMINAL 0x00 // nominal, no errors

void can_setup(void);

void can_receive_callback(const can_msg_t *msg);

void can_log(const can_msg_t *msg);

void send_status_ok(void);

#endif /* CAN_HANDLER_H */

