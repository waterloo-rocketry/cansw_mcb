#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

#define ADC_ANGLE_CONVERSION_FACTOR_hund_mdeg 3663 // 1/4095 * 15000 mdeg electrical angle * 100

void pot_init(void);

uint16_t pot_zero(void);

void pot_read(uint8_t channel);

uint16_t get_angle(uint16_t adc_value, uint16_t zero_value);

uint16_t filter_potentiometer(uint16_t new_reading);

#endif /* POTENTIOMETER_H */

