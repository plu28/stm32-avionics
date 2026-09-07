#include "stm32f446xx.h"
#include <stdbool.h>

int main(void) {
  // Enable the clock on the GPIOA bus
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN_Msk;

  // Set GPIOA pins to output
  // PA5 is set on bits 10-11 for this register
  GPIOA->MODER &= ~(3u << 10); // set bits 10-11 to 00
  GPIOA->MODER |= (1u << 10);  // set bits 10-11 to 01 (output)
  GPIOA->OTYPER &= ~(1u << 5); // set bit 5 to 0

  while (true) {
    // GPIOA->ODR ^= (1u << 5);

    GPIOA->BSRR |= (1u << 5); // set pin 5
    for (volatile int i = 0; i < 50000; i++) {
    }

    GPIOA->BSRR |= (1u << (16 + 5));

    for (volatile int i = 0; i < 500000; i++) {
    }
  }
}
