#ifndef IMU_H
#define IMU_H
#include <stdint.h>
void imu_init();

float get_x_accel();
float get_y_accel();
float get_z_accel();

int16_t get_roll();
int16_t get_yaw();
int16_t get_pitch();

#endif // !IMU_H
       

