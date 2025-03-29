#ifndef POTENTIOMETER_H
#define	POTENTIOMETER_H

#include <xc.h>
#include <stdint.h>

void pot_init(void);

void pot_read(uint8_t channel);



#endif	/* POTENTIOMETER_H */

