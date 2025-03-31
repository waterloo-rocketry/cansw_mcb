#ifndef POTENTIOMETER_H
#define	POTENTIOMETER_H

#include <xc.h>
#include <stdint.h>

#define CENTER_ADC_VALUE 0x800
#define SMOOTHING_FACTOR 0.2

void pot_init(void);

void pot_read(uint8_t channel);

uint16_t get_current_angle(uint16_t adc_value);

uint16_t get_filtered_angle(uint16_t current_angle, uint16_t prev_angle);

#endif	/* POTENTIOMETER_H */

