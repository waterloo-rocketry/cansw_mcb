#include "current_sensor.h"

void i2c_pin_init(void) {
    ANSELC3 = 0;
    ANSELC4 = 0;
    TRISC3 = 0;
    TRISC4 = 0;
    ODCC3 = 1;
    ODCC4 = 1;
    I2C1SCLPPS = 0b10100; //RC4
    I2C1SDAPPS = 0b10011; //RC3
    RC3PPS = 0b100010; //I2C1 SDA
    RC4PPS = 0b100001; //I2C1 SCL
}

uint16_t build_config_reg(void) {
    uint16_t config = 0;
    config |= RST;
    config |= RESERVED;
    config |= ADCRANGE;
    config |= AVG;
    config |= VBUSCT;
    config |= VSHCT;
    config |= MODE;
    return config; //0x4527
}

bool current_sense_init(void) {
    const uint16_t shunt_cal = 0.00512 / (CURRENT_LSB * R_SHUNT);
    bool config_success = 0;
    bool cal_success = 0;
    config_success = i2c_write_reg16(I2C_ADDR, CONFIG_REG, build_config_reg());
    cal_success = i2c_write_reg16(I2C_ADDR, CAL_REG, shunt_cal);
    return config_success && cal_success;
}

uint16_t current_read(void) {
    uint16_t value;
    i2c_read_reg16(I2C_ADDR, CURRENT_REG, &value);
    return CURRENT_LSB * value * 1000; //returns servo draw in mA
}

uint16_t voltage_read(void) {
    uint16_t value;
    i2c_read_reg16(I2C_ADDR, VOLTAGE_REG, &value);
    return VOLTAGE_LSB * value * 1000; //returns batt voltage in mV
}
