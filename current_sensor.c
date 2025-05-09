#include "current_sensor.h"

uint16_t build_config_reg(void) {
    uint16_t config = 0;
    config |= RST;
    config |= RESERVED;
    config |= ADCRANGE;
    config |= AVG;
    config |= VBUSCT;
    config |= VSHCT;
    config |= MODE;
    return config;
}

bool current_sense_init(void) {
    const uint16_t shunt_cal = 0.00512 / (CURRENT_LSB * R_SHUNT);
    bool config_success = 0;
    bool cal_success = 0;
    config_success = i2c_write_reg16(I2C_ADDR, CONFIG_REG, build_config_reg());
    cal_success = i2c_write_reg16(I2C_ADDR, CAL_REG, shunt_cal);
    return config_success && cal_success;
}

float current_read(void) {
    uint16_t value;
    i2c_read_reg16(I2C_ADDR, CURRENT_REG, &value);
    return CURRENT_LSB * value;
}

float voltage_read(void) {
    uint16_t value;
    i2c_read_reg16(I2C_ADDR, VOLTAGE_REG, &value);
    return VOLTAGE_LSB * value;
}

float filter_current(float new_reading) {
    const float alpha = 0.2;
    static float filtered_value = 0;

    if (filtered_value == 0) {
        filtered_value = new_reading;
    }

    filtered_value = alpha * new_reading + (1 - alpha) * filtered_value;
    return filtered_value;
}

float filter_voltage(float new_reading) {
    const float alpha = 0.2;
    static float filtered_value = 0;

    if (filtered_value == 0) {
        filtered_value = new_reading;
    }

    filtered_value = alpha * new_reading + (1 - alpha) * filtered_value;
    return filtered_value;
}