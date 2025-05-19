// #ifndef UART_H
// #define UART_H
// #include <stdint.h>


// void    uart_setup(void);
// void    uart_send_char(char c);
// void    uart_send_string(const char *str);
// void uart_send_hex8(uint8_t val);
// void uart_send_hex32(uint32_t val);
// void uart_send_uint(uint32_t val);

// #endif

#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_send_char(char c);
void uart_send_string(const char *str);
void uart_send_hex8(uint8_t val);
void uart_send_hex32(uint32_t val);
void uart_send_uint(uint32_t val);
void uart_setup(void);

#endif
