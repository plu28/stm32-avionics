#include <stdint.h>
#ifndef ESC

uint8_t get_ch1_throttle(); 
uint8_t get_ch2_throttle(); 
uint8_t get_ch3_throttle(); 
uint8_t get_ch4_throttle(); 
void esc_init();

void esc_set_throttle(uint8_t channel, uint8_t percent);

#endif // !ESC
