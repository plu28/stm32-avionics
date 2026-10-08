#include "esc.h"
#include "stm32f446xx.h"
#include "uart.h"

void esc_init() {
  // Enable TIM2 Peripheral Bus
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // Attach TIM2 to PB3
  GPIOB->MODER &= ~((3u << 6u)); // Clear PB3
  GPIOB->MODER |=
      (2u << 6u); // Set PB3 to alternate function mode
  GPIOB->AFR[0] &= ~(0xFu << (3 * 4)); // clear alternate function for PB3
  GPIOB->AFR[0] |=
      (0x1u << (3 * 4)); // set alternate function for PB3 to be AF1

  // Set prescaler
  TIM2->PSC = 15u;
  TIM2->ARR = 3999;
  TIM2->CCR2 = 1000; // minimum throttle

  // Set to PWM mode 1. CNT < CCR yields a high
  // TIM2->CCMR1 |= (TIM_CCMR1_OC2M & 6u);
  TIM2->CCMR1 &= ~(TIM_CCMR1_CC2S |
                   TIM_CCMR1_OC2M |
                   TIM_CCMR1_OC2PE);
  TIM2->CCMR1 |= (6u << 12u);

  TIM2->EGR |= TIM_EGR_UG;

  TIM2->CCER |= TIM_CCER_CC2E;
  TIM2->CCER &= ~TIM_CCER_CC2P; // active-low output enable

  TIM2->CR1 |= TIM_CR1_CEN; // enable
}

void set_ccr(uint16_t value) {
  // TIM2->CCR1 = value;
  TIM2->CCR2 = value;
  // TIM2->CCR3 = value;
  // TIM2->CCR4 = value;
}

void esc_set_throttle(uint8_t percent) {
  // percent /= 100;
  if (percent == 0) {
    set_ccr(1000);
  }
  if (percent > 100) {
    uart_printf("Can't set throttle beyond 100%? Got %d\n", percent);
    return;
  }
  // Calculate CCR value given throttle percent
  // set_ccr((0.25 + (0.005 * percent)) * 4000); // CCR = DC(ARR+1)
  set_ccr(1000u +(10u * percent));
}
