#include "i2c.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define IMU_ADDR 0x68u

#define X_ACCEL_LOW_ADDR 0x3Cu
#define X_ACCEL_HIGH_ADDR 0x3Bu
#define Y_ACCEL_LOW_ADDR 0x3Eu
#define Y_ACCEL_HIGH_ADDR 0x3Du
#define Z_ACCEL_LOW_ADDR 0x40u
#define Z_ACCEL_HIGH_ADDR 0x3Fu
#define ACCEL_RANGE_2 16384 // Range of +- 2 g
#define ACCEL_RANGE_4 8192  // Range of +- 4 g
#define ACCEL_RANGE_8 4096  // Range of +- 8 g
#define ACCEL_RANGE_16 2048 // Range of +- 16 g

#define X_GYRO_LOW_ADDR 44u
#define X_GYRO_HIGH_ADDR 43u
#define Y_GYRO_LOW_ADDR 46u
#define Y_GYRO_HIGH_ADDR 45u
#define Z_GYRO_LOW_ADDR 48u
#define Z_GYRO_HIGH_ADDR 47u
#define GYRO_RANGE_250 131   // Range of +- 250 deg/s
#define GYRO_RANGE_500 65.5  // Range of +- 500 deg/s
#define GYRO_RANGE_1000 32.8 // Range of +- 1000 deg/s
#define GYRO_RANGE_2000 16.4 // Range of +- 2000 deg/s

void imu_init(void) { i2c_init(); 
	// Configure IMU
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

float get_roll() {
  uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, X_GYRO_LOW_ADDR);
  uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, X_GYRO_HIGH_ADDR);
  int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
  return (float)raw / GYRO_RANGE_250;
}

float get_pitch() {
  uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, Y_GYRO_LOW_ADDR);
  uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, Y_GYRO_HIGH_ADDR);
  int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
  return (float)raw / GYRO_RANGE_250;
}

float get_yaw() {
  uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, Z_GYRO_LOW_ADDR);
  uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, Z_GYRO_HIGH_ADDR);
  int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
  return (float)raw / GYRO_RANGE_250;
}

