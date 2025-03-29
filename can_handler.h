#ifndef CAN_HANDLER_H
#define	CAN_HANDLER_H

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "canlib.h"
#include "message_types.h"
#include "pwm.h"

#define _XTAL_FREQ 12000000


void can_setup(void);

void can_receive_callback(const can_msg_t *msg);

void send_status_ok(void);

void can_log(const can_msg_t *msg);

#endif	/* CAN_HANDLER_H */

