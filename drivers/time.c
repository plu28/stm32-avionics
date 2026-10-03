#include "time.h"

volatile int milliseconds;

void SysTick_Handler() { milliseconds++; }

void delay(int ms) {
  int start = milliseconds;
  while (milliseconds < (start + ms)) {
  }
}

uint32_t millis() {
	return milliseconds;
}
