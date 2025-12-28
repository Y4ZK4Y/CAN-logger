#include "delay.h"

void delay_ms(volatile uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 8000; i++) {
        __asm__("nop");
    }
}
