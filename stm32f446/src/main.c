#include "../drivers/uart.h"
#include "../drivers/imu.h"
#include "stm32f446xx.h"
#include <stdbool.h>
#include <stdio.h>

volatile int milliseconds;

void SysTick_Handler() { milliseconds++; }

void delay(int ms) {
  int start = milliseconds;
  while (milliseconds < (start + ms)) {
  }
}

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
  imu_init();
}

int main(void) {
  system_init(); // Configures the clock to tick every 1ms

  uart_printstr("Keep MPU still! Calibrating...");
  GPIOA->BSRR |= (1u << 5); // turn LED on while calibrating

  imu_calibrate_gyro();

  GPIOA->BSRR |= (1u << (16 + 5)); // turn LED off
  uart_printstr("Calibration complete.\r\n");

  float x_accel, y_accel, z_accel, roll, pitch, yaw; 

  while (true) {
    // GPIOA->ODR ^= (1u << 5);
    x_accel = get_x_accel();
    y_accel = get_y_accel();
    z_accel = get_z_accel();
    roll = get_roll();
    pitch = get_pitch();
    yaw = get_yaw();

    uart_printf("\x1b[2K\rx:     %.2f g\r\n", x_accel);
    uart_printf("\x1b[2K\ry:     %.2f g\r\n", y_accel);
    uart_printf("\x1b[2K\rz:     %.2f g\r\n", z_accel);
    uart_printf("\x1b[2K\rroll:  %.2f deg/s\r\n", roll);
    uart_printf("\x1b[2K\rpitch: %.2f deg/s\r\n", pitch);
    uart_printf("\x1b[2K\ryaw:   %.2f deg/s\r\n", yaw);

    uart_printstr("\x1b[6A");
    delay(250);

  }
}
