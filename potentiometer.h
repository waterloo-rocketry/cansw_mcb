#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <stdint.h>
#include <xc.h>

#define POT_ELECTRICAL_ANGLE_mdeg 300
#define ADC_ANGLE_CONVERSION_FACTOR_mdeg 73.26 // 1/4095 * POT_ELECTRICAL_ANGLE_mdeg

void pot_init(void);

void pot_read(uint8_t channel);

uint16_t get_angle(int adc_value);

uint16_t filter_potentiometer(uint16_t new_reading);

#endif /* POTENTIOMETER_H */

