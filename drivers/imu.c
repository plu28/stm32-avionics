#include "stm32f446xx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "i2c.h"

#define IMU_ADDR 0x68u
#define X_ACCEL_LOW_ADDR 0x3Cu
#define X_ACCEL_HIGH_ADDR 0x3Bu
#define Y_ACCEL_LOW_ADDR 0x3Eu
#define Y_ACCEL_HIGH_ADDR 0x3Du
#define Z_ACCEL_LOW_ADDR 0x40u
#define Z_ACCEL_HIGH_ADDR 0x3Fu
#define ACCEL_RANGE_2 16384 // Range of +-2 g
#define ACCEL_RANGE_4 8192 // Range of +- 4 g
#define ACCEL_RANGE_8 4096 // Range of +- 8 g
#define ACCEL_RANGE_16 2048 // Range of +- 16 g

void imu_init(void) {
	i2c_init();
}

float get_x_accel() {
	uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, X_ACCEL_LOW_ADDR);
	uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, X_ACCEL_HIGH_ADDR);
	int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
	return (float)raw / ACCEL_RANGE_2;
}

float get_y_accel() {
	uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, Y_ACCEL_LOW_ADDR);
	uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, Y_ACCEL_HIGH_ADDR);
	int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
	return (float)raw / ACCEL_RANGE_2;
}

float get_z_accel() {
	uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, Z_ACCEL_LOW_ADDR);
	uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, Z_ACCEL_HIGH_ADDR);
	int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
	return (float)raw / ACCEL_RANGE_2;
}
