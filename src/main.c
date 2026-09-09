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
  float x_accel, y_accel, z_accel, roll, pitch, yaw; 

  while (true) {
    // GPIOA->ODR ^= (1u << 5);

    x_accel = get_x_accel();
    y_accel = get_y_accel();
    z_accel = get_z_accel();
    roll = get_x_accel();
    pitch = get_y_accel();
    yaw = get_z_accel();

    uart_printf("\x1b[2Kx: %.2f g\r\n", x_accel);
    uart_printf("\x1b[2Ky: %.2f g\r\n", y_accel);
    uart_printf("\x1b[2Kz: %.2f g\r\n", z_accel);
    uart_printf("\x1b[2Kroll: %.2f deg/s\r\n", roll);
    uart_printf("\x1b[2Kpitch: %.2f deg/s\r\n", pitch);
    uart_printf("\x1b[2Kyaw: %.2f deg/s\r", yaw);

    uart_printf("\x1b[5A"); // Move up 5 lines

    // delay(50);
    // Blink LED code
    GPIOA->BSRR |= (1u << 5); // set pin 5
    delay(100);
    GPIOA->BSRR |= (1u << (16 + 5));
    delay(100);

  }
}
