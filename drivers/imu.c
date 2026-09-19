#include "i2c.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void delay(int ms);

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

#define X_GYRO_LOW_ADDR 0x44u
#define X_GYRO_HIGH_ADDR 0x43u
#define Y_GYRO_LOW_ADDR 0x46u
#define Y_GYRO_HIGH_ADDR 0x45u
#define Z_GYRO_LOW_ADDR 0x48u
#define Z_GYRO_HIGH_ADDR 0x47u
#define GYRO_RANGE_250 131   // Range of +- 250 deg/s
#define GYRO_RANGE_500 65.5  // Range of +- 500 deg/s
#define GYRO_RANGE_1000 32.8 // Range of +- 1000 deg/s
#define GYRO_RANGE_2000 16.4 // Range of +- 2000 deg/s

void imu_init(void) {
	// Configure IMU
    i2c_init(); 
    
    // Wake IMU by writing 0 to PWR_MGMT_1 register
    i2c_write_byte(IMU_ADDR, 0x6Bu, 0x00u);
    delay(100);
}

float get_x_accel(void) {
  uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, X_ACCEL_HIGH_ADDR);
  uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, X_ACCEL_LOW_ADDR);
  int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
  return (float)raw / ACCEL_RANGE_2;
}

float get_y_accel(void) {
  uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, Y_ACCEL_HIGH_ADDR);
  uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, Y_ACCEL_LOW_ADDR);
  int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
  return (float)raw / ACCEL_RANGE_2;
}

float get_z_accel(void) {
  uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, Z_ACCEL_HIGH_ADDR);
  uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, Z_ACCEL_LOW_ADDR);
  int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
  return (float)raw / ACCEL_RANGE_2;
}

float get_raw_roll(void) {
  uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, X_GYRO_HIGH_ADDR);
  uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, X_GYRO_LOW_ADDR);
  int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
  return (float)raw / GYRO_RANGE_250;
}

float get_raw_pitch(void) {
  uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, Y_GYRO_HIGH_ADDR);
  uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, Y_GYRO_LOW_ADDR);
  int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
  return (float)raw / GYRO_RANGE_250;
}

float get_raw_yaw(void) {
  uint8_t higher_8_bytes = i2c_read_byte(IMU_ADDR, Z_GYRO_HIGH_ADDR);
  uint8_t lower_8_bytes = i2c_read_byte(IMU_ADDR, Z_GYRO_LOW_ADDR);
  int16_t raw = ((int16_t)higher_8_bytes << 8) | lower_8_bytes;
  return (float)raw / GYRO_RANGE_250;
}

static float gyro_x_offset = 0.0f;
static float gyro_y_offset = 0.0f;
static float gyro_z_offset = 0.0f;

void imu_calibrate_gyro(void) {
    const int samples = 500;

    float x_sum = 0.0f;
    float y_sum = 0.0f;
    float z_sum = 0.0f;

    for (int i = 0; i < samples; i++) {
        x_sum += get_raw_roll();
        y_sum += get_raw_pitch();
        z_sum += get_raw_yaw();
        delay(2);
    }

    gyro_x_offset = x_sum / samples;
    gyro_y_offset = y_sum / samples;
    gyro_z_offset = z_sum / samples;
}    

float get_roll(void) {
    return get_raw_roll() - gyro_x_offset;
}

float get_pitch(void) {
    return get_raw_pitch() - gyro_y_offset;
}

float get_yaw(void) {
    return get_raw_yaw() - gyro_z_offset;
}

