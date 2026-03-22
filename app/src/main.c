#include "common_defines.h"
#include "uart.h"
#include "system.h"
#include "can.h"
#include "sd.h"
#include "spi.h"
#include "logger.h"
#include <libopencm3/stm32/can.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "uart.h"


// int main(void) {
//     rcc_clock_setup_pll(&rcc_hsi16_configs[RCC_CLOCK_VRANGE1_80MHZ]);

//     uart_setup();
//      uart_send_string("uart okay\r\n");
//     spi_setup();
//      uart_send_string("spi okay.\r\n");

//     uart_send_string("Starting SD init...\r\n");

//     sd_card_init();

//     while (1);
// }


int main(void)
{
    clock_setup();
    //rcc_clock_setup_pll(&rcc_hsi16_configs[RCC_CLOCK_VRANGE1_80MHZ]);
    uart_setup();
    for (volatile int i = 0; i < 1000000; i++);  // crude ~100ms delay
    uart_send_string("UART test\r\n");

    //can_setup();
    spi_setup();
    uart_send_string("spi init done\r\n");
    sd_card_init();
    // uart_send_string("SPI test: sending 0xFF\r\n");
    // uint8_t x = spi_transfer(0xFF);  // You need to expose spi_transfer or add a test
    // uart_send_string("SPI read back: 0x");
    // uart_send_hex8(x);
    // uart_send_string("\r\n");
    // uart_send_string("SPI test done\r\n");
    //spi_send(SPI1, 0xFF);
    uart_send_string("SD init done\r\n");

     while (1) {
        uart_send_string("Hello\r\n");
        for (volatile int i = 0; i < 1000000; i++);  // crude delay
    }
}
