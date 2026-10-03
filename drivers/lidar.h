#ifndef LIDAR_H
#define IMU_H
#include <stdint.h>
void lidar_init(uint8_t range);

uint16_t get_lidar(uint8_t x, uint8_t y);

#endif // !IMU_H

