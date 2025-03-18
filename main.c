#include <xc.h>
#include "stdint.h"
#include "stdbool.h"     
#include "setup.h"
#include "canlib.h"
#include "can.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 12000000 //12MHz



int main(void) {
    pin_init();
    osc_init();
    if (OSCCON2bits.COSC != 0b011) {
        LATA1 = 0;
    }
    while(1) {
        CLRWDT();
        LATA0 = 0;
        __delay_ms(500);
        LATA0 = 1;
        __delay_ms(500);
    }
    
}