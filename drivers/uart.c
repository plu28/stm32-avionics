#include "esc.h"
#include "stm32f446xx.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#define MAX_UART_BUF                                                           \
  1000 // look bud if you're printing more than 1000 chasr just dont

static int uart_enabled_f = 0;

void uart_printf(char *format, ...);

void USART2_IRQHandler(void) {
  while (!(USART2->SR & USART_SR_RXNE)) {
  }
  uint8_t byte = USART2->DR;

  switch (byte) {
  case 'k':
    uart_printf("\r\nKilling all throttle...\r\n");
    esc_set_throttle(1, 0);
    esc_set_throttle(2, 0);
    esc_set_throttle(3, 0);
    esc_set_throttle(4, 0);
    break;
  case 'i':
    uart_printf("\r\nSetting throttle to idle (2%%)...\r\n");
    esc_set_throttle(1, 2);
    esc_set_throttle(2, 2);
    esc_set_throttle(3, 2);
    esc_set_throttle(4, 2);
    break;
  case '1':
    uart_printf("\r\nSetting throttle to 10%%...\r\n");
    esc_set_throttle(1, 10);
    esc_set_throttle(2, 10);
    esc_set_throttle(3, 10);
    esc_set_throttle(4, 10);
    break;
  case '3':
    uart_printf("\r\nSetting throttle to 30%%...\r\n");
    esc_set_throttle(1, 30);
    esc_set_throttle(2, 30);
    esc_set_throttle(3, 30);
    esc_set_throttle(4, 30);
    break;
  case '5':
    uart_printf("\r\nSetting throttle to 50%%...\r\n");
    esc_set_throttle(1, 50);
    esc_set_throttle(2, 50);
    esc_set_throttle(3, 50);
    esc_set_throttle(4, 50);
    break;
  case '9':
    uart_printf("\r\nSetting throttle to 90%%...\r\n");
    esc_set_throttle(1, 90);
    esc_set_throttle(2, 90);
    esc_set_throttle(3, 90);
    esc_set_throttle(4, 90);
    break;
  case 'm':
    uart_printf("\r\nSetting throttle to max 100%%...\r\n");
    esc_set_throttle(1, 90);
    esc_set_throttle(2, 90);
    esc_set_throttle(3, 90);
    esc_set_throttle(4, 90);
    break;
  default:
      uart_printf("\r\nUnknown command: %c\r\n", byte);
  }
}

void uart_init() {

  RCC->APB1ENR |= RCC_APB1ENR_USART2EN_Msk; // Enable clock for the peripheral
                                            // bus that USART2 is on
  // RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN_Msk;

  USART2->CR1 &=
      ~(1u << 12u);     // Set the word length to be 8 bits (a byte)
                        // Leaving stop bit amount at default value (1 stop bit)
  USART2->BRR = (0x8B); // Setting baud rate to 115200. 16MHz / 115200 = ~139 =
                        // 8B 0x8B (this is the mantissa) 0x8B

  USART2->CR1 |= USART_CR1_TE; // Set TE bit to be enabled (transmitter enabled)
  USART2->CR1 |= USART_CR1_RE; // Set RE bit to be enabled (receiver enabled)
  USART2->CR1 |= USART_CR1_RXNEIE; // Enable interrupts

  // Set PA2 to alternate function so it trasmits to the ST-Link
  GPIOA->MODER &= ~(3u << 4); // set bits 3-4 to 00
  GPIOA->MODER |= (2u << 4);  // set bits 3-4 to 10 (alternate function)
  GPIOA->AFR[0] &= ~(0xFu << (2 * 4)); // clear alternate function
  GPIOA->AFR[0] |= (7u << (2 * 4));    // set to alternate function 7

  // Set PA3 to alternate function 7 so it receives
  GPIOA->MODER &= ~(3u << 6); // set bits 5-6 to 00
  GPIOA->MODER |= (2u << 6);  // set bits 5-6 to 10 (alternate function)
  GPIOA->AFR[0] &= ~(0xFu << (3 * 4)); // clear alternate function
  GPIOA->AFR[0] |= (7u << (3 * 4));    // set to alternate function 7

  NVIC_EnableIRQ(USART2_IRQn);

  USART2->CR1 |= USART_CR1_UE; // Enable USART

  uart_enabled_f = 1;
}

void uart_print_byte(char c) {
  if (!uart_enabled_f) {
    return;
  }

  while (!(USART2->SR & USART_SR_TXE)) {
  }
  USART2->DR = c;
}

// Prints a string to the uart terminal
// String must be null terminated
// Prints a new line at the end
void uart_printstr(char *s) {
  char *curr = s;
  while (*curr != '\0') {
    uart_print_byte(*curr);
    curr++;
  }
  // uart_print_byte('\0');
  // uart_print_byte('\n');
  // uart_print_byte('\r');
}

void uart_printnum(int16_t n) {
  char buf[100];
  snprintf(buf, sizeof(buf), "%hd", n);
  uart_printstr(buf);

  // char* curr = s;
  // while (*curr != '\0') {
  //   uart_print_byte(*curr);
  //   curr++;
  // }
  // uart_print_byte('\0');
  // uart_print_byte('\n');
  // uart_print_byte('\r');
}

void uart_printf(char *format, ...) {
  char buf[MAX_UART_BUF];

  va_list args;
  va_start(args, format);
  vsnprintf(buf, MAX_UART_BUF, format, args);
  va_end(args);

  uart_printstr(buf);
}
