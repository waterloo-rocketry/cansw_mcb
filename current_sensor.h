#ifndef CURRENT_SENSOR_H
#define CURRENT_SENSOR_H

#include "i2c.h"
#include <stdbool.h>
#include <stdint.h>
#include <xc.h>

#define I2C_ADDR 0b1000000
#define CURRENT_REG 0x04
#define VOLTAGE_REG 0x02
#define CAL_REG 0x05
#define CONFIG_REG 0x00

// Config register bits
#define RST (0b0 << 15) // Bit 15 is 0b for normal operation
#define RESERVED (0b10 << 13) // Bits 14-13 must be 10
#define ADCRANGE (0b0 << 12) // Bit 12: 0 for ±81.92 mV
#define AVG (0b010 << 9) // Bits 11-9: 010b for 16 samples averaging
#define VBUSCT (0b100 << 6) // Bits 8-6: 100b for 1100 µs bus conversion time
#define VSHCT (0b100 << 3) // Bits 5-3: 100b for 1100 µs shunt conversion time
#define MODE (0b111 << 0) // Bits 2-0: 111b for continuous shunt and bus voltage mode

// Calibration
#define MAX_CURRENT 5.0
#define R_SHUNT 0.013
#define CURRENT_LSB                                                                                \
    (MAX_CURRENT * 2 / 32768) // Minimum LSB is max current/2^15 per datasheet. 2x safety factor

#define VOLTAGE_LSB 0.0016

void i2c_pin_init(void);
uint16_t build_config_reg(void);
bool current_sense_init(void);
float current_read(void);
float voltage_read(void);

#endif /* CURRENT_SENSOR_H */

