#include "stm32f446xx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "i2c.h"

#define IMU_ADDR 0x68u
#define X_ACCEL_LOW_ADDR 0x3Cu
#define X_ACCEL_HIGH_ADDR 0x3Bu

void imu_init(void) {
	i2c_init();
}

int16_t get_x_accel() {
	uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, X_ACCEL_LOW_ADDR);
	uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, X_ACCEL_HIGH_ADDR);
	int16_t ret = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
	return ret;
}
