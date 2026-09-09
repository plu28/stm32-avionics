#include <stdint.h>
#ifndef I2C_H

// Read a single byte given a peripheral address and a register address
uint8_t i2c_read_byte(uint8_t i2c_addr, uint8_t reg_addr);

// Initialize I2C
void i2c_init();

#endif // !I2C_H
