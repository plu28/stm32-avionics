#ifndef LIDAR_H
#define IMU_H
#include <stdint.h>
typedef uint16_t lidar_point_t[2]; // [x, y] 
typedef lidar_point_t x8_lidar_t[64]; // 8x8 lidar data
typedef lidar_point_t x4_lidar_t[16]; // 4x4 lidar data


void lidar_init(uint8_t range);

uint16_t get_lidar(uint8_t x, uint8_t y);

// Get all 8x8 data
void get_x8_lidar(x8_lidar_t* data);

// Get all 4x4 data
void get_x4_lidar(x4_lidar_t* data);

#endif // !IMU_H

