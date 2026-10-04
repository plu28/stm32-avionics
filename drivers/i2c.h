#include <stdint.h>
#ifndef I2C_H
#define I2C_H

// initialize I2C
void i2c_init();

// Read/Write a single byte given a peripheral address and a register address
uint8_t i2c_read_reg_byte(uint8_t i2c_addr, uint8_t reg_addr);
void i2c_write_reg_byte(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);

void i2c_read(uint8_t i2c_addr, int16_t reg_addr, void* result, uint32_t nbytes); // Read an arbitrary amount of bytes and store in result.
void i2c_write(uint8_t i2c_addr, int16_t reg_addr, uint8_t* payload, uint32_t size); // Write an arbitrary amount of bytes


#endif // !I2C_H
