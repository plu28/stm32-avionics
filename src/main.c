#include "../drivers/uart.h"
#include "../drivers/imu.h"
#include "../drivers/lidar.h"
#include "../drivers/i2c.h"
#include "../drivers/time.h"
#include "stm32f446xx.h"
#include <stdbool.h>
#include <stdio.h>

static void system_init(void) {
  // Enable the clock on the GPIOA, GPIOB, and i2c busses
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN_Msk;
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN_Msk;
  // Set GPIOA pins to output
  // PA5 is set on bits 10-11 for this register
  GPIOA->MODER &= ~(3u << 10);   // set bits 10-11 to 00
  GPIOA->MODER |= (1u << 10);    // set bits 10-11 to 01 (output)
  GPIOA->OTYPER &= ~(1u << 5);   // set bit 5 to 0
  GPIOA->OSPEEDR &= ~(3u << 10); // set speed bits to 00
  // GPIOA->OSPEEDR |= (1u << 00); // low speed (00)
  // GPIOA->OSPEEDR |= (1u << 10);  // medium speed (01)
  // GPIOA->OSPEEDR |= (1u << 11); // high speed (11)
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000u);

  uart_init();
  i2c_init();
  lidar_init(8);
  // imu_init();
}

int main(void) {
  system_init(); // Configures the clock to tick every 1ms

  // uart_printstr("Keep MPU still! Calibrating...");
  // GPIOA->BSRR |= (1u << 5); // turn LED on while calibrating
  //
  // imu_calibrate_gyro();
  //
  // GPIOA->BSRR |= (1u << (16 + 5)); // turn LED off
  // uart_printstr("Calibration complete.\r\n");
  //
  // float x_accel, y_accel, z_accel, roll, pitch, yaw;

  while (true) {
    GPIOA->ODR ^= (1u << 5);
    // x_accel = get_x_accel();
    // y_accel = get_y_accel();
    // z_accel = get_z_accel();
    // roll = get_roll();
    // pitch = get_pitch();
    // yaw = get_yaw();

    // uart_printf("\x1b[2K\rx:     %.2f g\r\n", x_accel);
    // uart_printf("\x1b[2K\ry:     %.2f g\r\n", y_accel);
    // uart_printf("\x1b[2K\rz:     %.2f g\r\n", z_accel);
    // uart_printf("\x1b[2K\rroll:  %.2f deg/s\r\n", roll);
    // uart_printf("\x1b[2K\rpitch: %.2f deg/s\r\n", pitch);
    // uart_printf("\x1b[2K\ryaw:   %.2f deg/s\r\n", yaw);
    uart_printf("\x1b[2K\rDepth Map (mm):\r\n");
    uart_printf("\x1b[2K\r%d %d %d %d %d %d %d %d\r\n", get_lidar(0,0), get_lidar(1,0), get_lidar(2,0), get_lidar(3,0), get_lidar(4,0), get_lidar(5,0), get_lidar(6,0), get_lidar(7,0));
    uart_printf("\x1b[2K\r%d %d %d %d %d %d %d %d\r\n", get_lidar(0,1), get_lidar(1,1), get_lidar(2,1), get_lidar(3,1), get_lidar(4,1), get_lidar(5,1), get_lidar(6,1), get_lidar(7,1));
    uart_printf("\x1b[2K\r%d %d %d %d %d %d %d %d\r\n", get_lidar(0,2), get_lidar(1,2), get_lidar(2,2), get_lidar(3,2), get_lidar(4,2), get_lidar(5,2), get_lidar(6,2), get_lidar(7,2));
    uart_printf("\x1b[2K\r%d %d %d %d %d %d %d %d\r\n", get_lidar(0,3), get_lidar(1,3), get_lidar(2,3), get_lidar(3,3), get_lidar(4,3), get_lidar(5,3), get_lidar(6,3), get_lidar(7,3));
    uart_printf("\x1b[2K\r%d %d %d %d %d %d %d %d\r\n", get_lidar(0,4), get_lidar(1,4), get_lidar(2,4), get_lidar(3,4), get_lidar(4,4), get_lidar(5,4), get_lidar(6,4), get_lidar(7,4));
    uart_printf("\x1b[2K\r%d %d %d %d %d %d %d %d\r\n", get_lidar(0,5), get_lidar(1,5), get_lidar(2,5), get_lidar(3,5), get_lidar(4,5), get_lidar(5,5), get_lidar(6,5), get_lidar(7,5));
    uart_printf("\x1b[2K\r%d %d %d %d %d %d %d %d\r\n", get_lidar(0,6), get_lidar(1,6), get_lidar(2,6), get_lidar(3,6), get_lidar(4,6), get_lidar(5,6), get_lidar(6,6), get_lidar(7,6));
    uart_printf("\x1b[2K\r%d %d %d %d %d %d %d %d\r\n", get_lidar(0,7), get_lidar(1,7), get_lidar(2,7), get_lidar(3,7), get_lidar(4,7), get_lidar(5,7), get_lidar(6,7), get_lidar(7,7));

    uart_printstr("\x1b[9A");
    delay(250);

  }
}
