#include "stm32f446xx.h"
#include <stdbool.h>

volatile int milliseconds;

void SysTick_Handler() { milliseconds++; }

void delay(int ms) {
  int start = milliseconds;
  while (milliseconds < (start + ms)) {
  }
}

static void system_init(void) {
  SystemCoreClockUpdate();

  SysTick_Config(SystemCoreClock / 1000u);
}

int main(void) {
  system_init(); // Configures the clock to tick every 1ms

  // Enable the clock on the GPIOA bus
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN_Msk;

  // Set GPIOA pins to output
  // PA5 is set on bits 10-11 for this register
  GPIOA->MODER &= ~(3u << 10); // set bits 10-11 to 00
  GPIOA->MODER |= (1u << 10);  // set bits 10-11 to 01 (output)
  GPIOA->OTYPER &= ~(1u << 5); // set bit 5 to 0
  // GPIOA->OSPEEDR &= ~(3u << 10); // set speed bits to 00
  // GPIOA->OSPEEDR |= (1u << 00); // low speed (00)
  // GPIOA->OSPEEDR |= (1u << 10);  // medium speed (01)
  // GPIOA->OSPEEDR |= (1u << 11); // high speed (11)

  while (true) {
    // GPIOA->ODR ^= (1u << 5);

    GPIOA->BSRR |= (1u << 5); // set pin 5
    delay(1000);
    // for (volatile int i = 0; i < 50000; i++) {
    // }

    GPIOA->BSRR |= (1u << (16 + 5));
    delay(1000);

    // for (volatile int i = 0; i < 500000; i++) {
    // }
  }
}
