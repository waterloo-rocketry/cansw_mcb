#include "can_handler.h"

// memory pool for the CAN tx buffer
uint8_t tx_pool[500];
extern uint16_t cmd_angle;
extern bool new_cmd;

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
    // set up CAN tx buffer
    txb_init(tx_pool, sizeof(tx_pool), can_send, can_send_rdy);
}

void can_receive_callback(const can_msg_t *msg) {
    
    // ignore messages that were sent from this board (Primary and Failsafe)
    if (get_board_type_unique_id(msg) == BOARD_TYPE_UNIQUE_ID) {
        return;
    }
    
    uint16_t msg_type = get_message_type(msg);
    uint16_t actuator_id;
    uint16_t actuator_state;
    
    switch(msg_type) {

#if (BOARD_INST_UNIQUE_ID == PRIMARY) 
        case MSG_ACTUATOR_ANALOG_CMD:
            actuator_id = get_actuator_id(msg);
            if (actuator_id != ACTUATOR_CANARD_ANGLE) {
                break;
            }
            actuator_state = get_cmd_actuator_state_analog (msg);
            if (actuator_state >= 22768 && actuator_state <= 42768) {
                cmd_angle = actuator_state;
                new_cmd = 1;
                
            }            
            break;
       
#elif (BOARD_INST_UNIQUE_ID == FAILSAFE)
        case MSG_ACTUATOR_CMD:
            actuator_id = get_actuator_id(msg);
            if (actuator_id == ACTUATOR_CANARD_ENABLE) {
                actuator_state = get_cmd_actuator_state(msg);
                if (actuator_state == ACT_STATE_ON) {
                    LATC5 = 0;
                }
                else if (actuator_state == ACT_STATE_OFF) {
                    LATC5 = 1;
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

void send_status_ok(void) {
    can_msg_t board_stat_msg;
    build_general_board_status_msg(PRIO_MEDIUM, millis(), E_NOMINAL, 0, &board_stat_msg);
    txb_enqueue(&board_stat_msg);
}


