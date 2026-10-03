#ifndef TIME
#include <stdint.h>


// Overwrite systick handler to increment milliseconds
void SysTick_Handler();

// Get the number of ticks in milliseconds
uint32_t millis();

// Stop time for a set amount of time
void delay(int ms);

#endif // !TIME
