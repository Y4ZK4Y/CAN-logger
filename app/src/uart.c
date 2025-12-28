#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "uart.h"

#define BAUD_RATE 115200

void uart_setup(void) {
    // clock setup
    rcc_periph_clock_enable(RCC_GPIOA); // A or D?
    rcc_periph_clock_enable(RCC_USART2); // 1 or 2?

    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2); // pin make sure
    gpio_set_af(GPIOA, GPIO_AF7, GPIO2); // pin num ensure

    usart_set_baudrate(USART2, BAUD_RATE);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX); // TX?
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    usart_enable(USART2);
}


void uart_send_char(char c) {
    usart_send_blocking(USART2, c);
}

void uart_send_string(const char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

// Convert nibble to hex char
static char hex_digit(uint8_t nibble) {
    return (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
}

void uart_send_hex8(uint8_t val) {
    const char hex_chars[] = "0123456789ABCDEF";
    uart_send_char(hex_chars[(val >> 4) & 0xF]);
    uart_send_char(hex_chars[val & 0xF]);
}


void uart_send_hex32(uint32_t val) {
    for (int i = 28; i >= 0; i -= 4) {
        uart_send_char(hex_digit((val >> i) & 0xF));
    }
}

void uart_send_uint(uint32_t val) {
    char buf[10];
    unsigned int i = 0;

    if (val == 0) {
        uart_send_char('0');
        return;
    }

    while (val && i < sizeof(buf)) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    while (i--) {
        uart_send_char(buf[i]);
    }
}
