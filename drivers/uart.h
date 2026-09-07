#ifndef UART_H
#define UART_H

// One time configuration of UART registers
void enable_uart();

void uart_printf(char* format, ...);



#endif // !UART_H
