#include "esc.h"
#include "stm32f446xx.h"
#include "uart.h"

static uint8_t ch1_throttle;
static uint8_t ch2_throttle;
static uint8_t ch3_throttle;
static uint8_t ch4_throttle;

void esc_init() {
  // Enable TIM3 Peripheral Bus
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  // PA6, PA7, PB0, PB1
  // Attach TIM3 Channel 1 to PA6 (yellow)
  GPIOA->MODER &= ~((3u << (6u * 2u))); // Clear PA6
  GPIOA->MODER |= (2u << (6u * 2u));    // Set PA6 to alternate function mode
  GPIOA->AFR[0] &= ~(0xFu << (6 * 4));  // clear alternate function for PA6
  GPIOA->AFR[0] |=
      (0x2u << (6 * 4)); // set alternate function for PA6 to be AF2

  // Attach TIM3 Channel 2 to PA7 (orange)
  GPIOA->MODER &= ~((3u << (7u * 2u))); // Clear PA7
  GPIOA->MODER |= (2u << (7u * 2u));    // Set PA7 to alternate function mode
  GPIOA->AFR[0] &= ~(0xFu << (7 * 4));  // clear alternate function for PA7
  GPIOA->AFR[0] |=
      (0x2u << (7 * 4)); // set alternate function for PA7 to be AF2

  // Attach TIM3 Channel 3 to PB0 (green)
  GPIOB->MODER &= ~((3u << (0u * 2u)));  // Clear PB0
  GPIOB->MODER |= (2u << (0u * 2u));     // Set PB0 to alternate function mode
  GPIOB->AFR[0] &= ~(0xFu << (0u * 4u)); // clear alternate function for PB0
  GPIOB->AFR[0] |=
      (0x2u << (0u * 4u)); // set alternate function for PB0 to be AF2

  // Attach TIM3 Channel 4 to PB1 (white)
  GPIOB->MODER &= ~((3u << (1u * 2u))); // Clear PB1
  GPIOB->MODER |= (2u << (1u * 2u));    // Set PB1 to alternate function mode
  GPIOB->AFR[0] &= ~(0xFu << (1u * 4)); // clear alternate function for PB1
  GPIOB->AFR[0] |=
      (0x2u << (1 * 4)); // set alternate function for PB1 to be AF2

  // Set prescaler
  TIM3->PSC = 15u;
  TIM3->ARR = 3999;
  // ARM the motors
  uart_printf("Arming motors...\r\n");
  TIM3->CCR1 = 1000;
  TIM3->CCR2 = 1000;
  TIM3->CCR3 = 1000;
  TIM3->CCR4 = 1000;

  /* Configure channels 1 and 2 with CCMR1 */
  // Set to PWM mode 1. CNT < CCR yields a high
  // TIM3->CCMR1 |= (TIM_CCMR1_OC2M & 6u);
  TIM3->CCMR1 &= ~(TIM_CCMR1_CC2S | TIM_CCMR1_OC2M | TIM_CCMR1_OC2PE |
                   TIM_CCMR1_CC1S | TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE);
  // select PWM mode 1
  TIM3->CCMR1 |= (6u << TIM_CCMR1_OC2M_Pos);
  TIM3->CCMR1 |= (6u << TIM_CCMR1_OC1M_Pos);

  // OC2PE → buffers CCR2 → PWM pulse width/duty cycle
  TIM3->CCMR1 |= TIM_CCMR1_OC2PE;
  TIM3->CCMR1 |= TIM_CCMR1_OC1PE;

  /* Configure channels 3 and 4 with CCMR2 */
  TIM3->CCMR2 &= ~(TIM_CCMR2_CC4S | TIM_CCMR2_OC4M | TIM_CCMR2_OC4PE |
                   TIM_CCMR2_CC3S | TIM_CCMR2_OC3M | TIM_CCMR2_OC3PE);
  // select PWM mode 1
  TIM3->CCMR2 |= (6u << TIM_CCMR2_OC3M_Pos);
  TIM3->CCMR2 |= (6u << TIM_CCMR2_OC4M_Pos);

  // OC2PE → buffers CCR2 → PWM pulse width/duty cycle
  TIM3->CCMR2 |= TIM_CCMR2_OC4PE;
  TIM3->CCMR2 |= TIM_CCMR2_OC3PE;

  /* Begin signal generation */

  TIM3->CR1 |= TIM_CR1_ARPE; // ARPE  → buffers ARR  → PWM period/frequency

  // active-high polarity output enable
  TIM3->CCER &=
      ~(TIM_CCER_CC1P | TIM_CCER_CC2P | TIM_CCER_CC3P | TIM_CCER_CC4P);

  // capture/compare enable
  TIM3->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E);

  TIM3->EGR = TIM_EGR_UG; // generates an update event

  TIM3->CR1 |= TIM_CR1_CEN; // enable
}

void esc_set_throttle(uint8_t channel, uint8_t percent) {
  if (percent > 100) {
    uart_printf("Can't set throttle beyond 100%? Got %d\r\n", percent);
    return;
  }

  uint32_t ccr_val = 1140u + (10u * percent);
  uart_printf("Setting throttle to %d%% for channel %d (CCR=%d)\r\n", percent,
              channel, ccr_val);

  switch (channel) {
  case 1:
    TIM3->CCR1 = ccr_val;
    ch1_throttle = percent;
    break;
  case 2:
    TIM3->CCR2 = ccr_val;
    ch2_throttle = percent;
    break;
  case 3:
    TIM3->CCR3 = ccr_val;
    ch3_throttle = percent;
    break;
  case 4:
    TIM3->CCR4 = ccr_val;
    ch4_throttle = percent;
    break;
  default:
    uart_printf("Tried to set throttle for channel %d?\r\n", channel);
    return;
  }
}

uint8_t get_ch1_throttle() {
  return ch1_throttle;
}

uint8_t get_ch2_throttle() {
  return ch2_throttle;
}

uint8_t get_ch3_throttle() {
  return ch3_throttle;
}

uint8_t get_ch4_throttle() {
  return ch4_throttle;
}
