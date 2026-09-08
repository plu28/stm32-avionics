#ifndef UART_H
#define UART_H

// One time configuration of UART registers
void uart_init();

void uart_printf(char *format, ...);

void uart_print_byte(char b);

void uart_print(char *s); 


#endif // !UART_H
