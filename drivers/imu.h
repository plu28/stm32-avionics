#ifndef IMU_H
#define IMU_H
#include <stdint.h>
void imu_init();
void imu_calibrate_gyro(void);

float get_x_accel();
float get_y_accel();
float get_z_accel();

float get_roll();
float get_yaw();
float get_pitch();

#endif // !IMU_H
       

