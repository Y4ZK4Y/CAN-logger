#include "spi.h"
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdint.h>
#include "uart.h"

/*
SPA PINS 
CS:PA_4(D4)     MISO: PA_6(D6)     MOSI:PA_7(D7)   SCK:PA_5(D5)
*/
#define SPI1_SCK    (GPIO5)
#define SPI1_MISO   (GPIO6)
#define SPI1_MOSI   (GPIO7)
#define SPI1_CS     (GPIO4)
#define SPI1_PORT   (GPIOA)


// void spi_setup(void) {
//     rcc_periph_clock_enable(RCC_GPIOA);
//     rcc_periph_clock_enable(RCC_SPI1);
    
//     // sck mosi af
//     gpio_mode_setup(SPI1_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI1_SCK | SPI1_MOSI);
//     gpio_set_af(SPI1_PORT, GPIO_AF5, SPI1_SCK | SPI1_MOSI);

//     // miso af input with pullup
//     gpio_mode_setup(SPI1_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI1_MISO);
//     gpio_set_af(SPI1_PORT, GPIO_AF5, SPI1_MISO);

//     // --- CS: Manual control (GPIO output push-pull) ---
//     gpio_mode_setup(SPI1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI1_CS);
//     gpio_set_output_options(SPI1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, SPI1_CS);
//     gpio_set(SPI1_PORT, SPI1_CS);  // Deselect device initially (CS = HIGH)

//     //spi_reset(SPI1);
//     //rcc_periph_reset_pulse(RST_SPI1);
//     spi_init_master(SPI1,
//                     SPI_CR1_BAUDRATE_FPCLK_DIV_64, // ~300 kHz
//                     SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, 
//                     SPI_CR1_CPHA_CLK_TRANSITION_1,
//                     SPI_CR1_MSBFIRST); 
//     spi_enable_software_slave_management(SPI1); //SSM = 1
//     spi_set_nss_high(SPI1); // SSI = 1 (simulate idle) - to bypass hardware NSS rules
//     spi_enable(SPI1);
// }


// uint8_t spi_transfer(uint8_t data) {
//     uart_send_string("Sending SPI: ");
//     uart_send_hex8(data);
//     uart_send_string("\r\n");

//     spi_send(SPI1, data);
//     uint8_t r = spi_read(SPI1);

//     uart_send_string("SPI RX: ");
//     uart_send_hex8(r);
//     uart_send_string("\r\n");

//     return r;
//}


// /* without log */
// uint8_t spi_transfer(uint8_t data) {
//     spi_send(SPI1, data);
//     return spi_read(SPI1);
// }


void spi_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_SPI1);

    gpio_mode_setup(SPI1_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI1_MISO |SPI1_SCK | SPI1_MOSI);
    gpio_set_af(SPI1_PORT, GPIO_AF5, SPI1_MISO |SPI1_SCK | SPI1_MOSI);

    //spi_reset(SPI1);
    spi_init_master(SPI1,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_256, // slow start for sd init - br - clock speed, divided by 256 Slow clock at init (≤ 400 kHz recommended; 48 MHz ÷ 256 ≈ 187 kHz)
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, //cpol - clock idle level low - 0
                    SPI_CR1_CPHA_CLK_TRANSITION_1, // cpha - data read on first edge - 0
                    //SPI_CR1_DFF_8BIT, // 8bit frame size - DFF is not required on STM32L4 — it's always 8-bit for libopencm3 unless you explicitly configure for 16-bit (which we’re not doing).
                    SPI_CR1_MSBFIRST); // lsbfirst - here msb first
    //spi_enable_software_slave_management(SPI1);
    //spi_set_nss_high(SPI1); // default cs high
    spi_enable(SPI1);
}
