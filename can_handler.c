#include "can_handler.h"

void can_setup(void) {
    CANRXPPS = 0x11;    // CAN receive pin = RC1
    RC0PPS = 0x33;      // CAN transmit pin = RC0

    // init gpio pins
    // tx
    TRISC0 = 0; // out
    LATC0 = 1;
    ODCC0 = 1;
    ANSELC0 = 0;

    // rx
    TRISC1 = 1; // in
    ANSELC1 = 0;

    can_timing_t timing;
    can_generate_timing_params(_XTAL_FREQ, &timing);
    can_init(&timing, can_receive_callback);
    
}

void can_receive_callback(const can_msg_t *msg) {
    // ignore messages that were sent from this board (Primary and Failsafe)
    if (get_board_type_unique_id(msg) == BOARD_TYPE_UNIQUE_ID) {
        return;
    }
    uint16_t msg_type = get_message_type(msg);
    int actuator_id;
    int actuator_state;
    
    switch(msg_type) {
        
#if (BOARD_INST_UNIQUE_ID == BOARD_INST_ID_CANARD_MOTOR_PRIMARY)
        case MSG_ACTUATOR_ANALOG_CMD:
            actuator_state = get_cmd_actuator_state_analog (msg);
            if (actuator_state >= 0 && actuator_state <=200) {
                updatePulseWidth(actuator_state);
            }
            break;
            
#elif (BOARD_INST_UNIQUE_ID == BOARD_INST_ID_CANARD_MOTOR_FAILSAFE)
        case MSG_ACTUATOR_CMD:
            actuator_id = get_actuator_id(msg);
            if (actuator_id == ACTUATOR_CANARD_ENABLE) {
                actuator_state = get_cmd_actuator_state(msg);
                if (actuator_state == ACT_STATE_ON) {
                    TRISC5 = 0;
                }
                else if (actuator_state == ACT_STATE_OFF) {
                    TRISC5 = 1;
                }
            }
            break;
            
#endif
            
        case MSG_LEDS_ON:
            LATA0 = 0;
            LATA1 = 0;
            break;
            
        case MSG_LEDS_OFF:
            LATA0 = 1;
            LATA1 = 1;
            break;
            
        case MSG_RESET_CMD:
            if (check_board_need_reset(msg)) {
                RESET();
            }    
            break;
            
        default:
            break;
            
            
    }
   
}
