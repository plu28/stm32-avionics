#include <stdint.h>
#ifndef I2C_H
#define I2C_H

// initialize I2C
void i2c_init();

// Read/Write a single byte given a peripheral address and a register address
uint8_t i2c_read_byte(uint8_t i2c_addr, uint8_t reg_addr);
void i2c_write_byte(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);

// Read an arbitrary amount of bytes and store in result
void i2c_read(uint8_t i2c_addr, uint8_t reg_addr, int n, void* result); 


#endif // !I2C_H
