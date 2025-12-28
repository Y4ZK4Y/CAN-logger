#include "sd.h"
#include "uart.h"
#include "delay.h"
#include "spi.h"
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <stdint.h>

#define SPI1_SCK    GPIO5
#define SPI1_MISO   GPIO6
#define SPI1_MOSI   GPIO7
#define SPI1_CS     GPIO4
#define SPI1_PORT   GPIOA


// uint8_t sd_send_command(uint8_t cmd, uint32_t arg, uint8_t crc) {
//     uint8_t response;
//     for (volatile int i = 0; i < 100000; i++); // crude delay ~ few ms

//     gpio_clear(SPI1_PORT, SPI1_CS); // CS low
//     spi_transfer(0xFF); // 1 dummy byte (delay) - 

//     // command packet
//     spi_transfer(0x40 | cmd); // Command token
//     spi_transfer(arg >> 24);
//     spi_transfer(arg >> 16);
//     spi_transfer(arg >> 8);
//     spi_transfer(arg & 0xFF);
//     spi_transfer(crc); // CRC (valid for CMD0 and CMD8)

//     // Wait for a valid response (max 8 bytes)
//     for (int i = 0; i < 10; i++) {
//         response = spi_transfer(0xFF);
//         if ((response & 0x80) == 0)
//             break ;
//     }

//     gpio_set(SPI1_PORT, SPI1_CS); // pull cs high - deselect card
//     spi_transfer(0xFF); // one more 8 clock cycle

//     return response;
// }

// void sd_card_init(void) {
//     uart_send_string("isnide cd_init func\r\n");
//     gpio_set(SPI1_PORT, SPI1_CS); // CS high - 80 dummy clocks with MOSI high
//     for (int i = 0; i < 10; i++) {
//         spi_transfer(0xFF);
//     }

//     uart_send_string("Sending CMD0(reset)...\r\n");
//     uint8_t r = sd_send_command(0, 0, 0x95); // CMD0 CRC= 0x95
//     uart_send_string("CMD0 Response: ");
//     uart_send_hex8(r);
//     uart_send_string("\r\n");

//     if (r != 0x01) {
//         uart_send_string("Card did not enter idle.\r\n");
//         return;
//     }

//     uart_send_string("Sending CMD8(voltage check)...\r\n");
//     r = sd_send_command(8, 0x1AA, 0x87); // CMD8
//     uart_send_string("CMD8 Response: ");
//     uart_send_hex8(r);
//     uart_send_string("\r\n");

//     // // Read rest of R7 (only if r == 0x01)
//     // if (r == 0x01) {
//     //     for (int i = 0; i < 4; i++) {
//     //         uint8_t v = spi_transfer(0xFF);
//     //         uart_send_hex8(v);
//     //         uart_send_string(" ");
//     //     }
//     //     uart_send_string("\r\n");
//     // }
// }




// tells sd card im not talking to you
static void cs_high(void) {
    gpio_set(SPI1_PORT, SPI1_CS);
}

// tells the sd card Im talking to you now
static void cs_low(void) {
    gpio_clear(SPI1_PORT, SPI1_CS);
}



// standrad pattern for full duplex spi - send one byte - simoultaneously rad one byte 
static uint8_t spi_transfer(uint8_t data) {
    uart_send_string("isnide spi_tarnsfer func: \r\n");
    spi_send(SPI1, data);
    uart_send_string("end of spi_tarnsfer func: \r\n");
    return spi_read(SPI1);
}

// sends 80 spi clock pulses (10 x 8 bits) with cs high - required by sd card specs top enter spi mode
static void sd_send_dummy_clock(void) {
    uart_send_string("isnide send dummy func\r\n");
    cs_high(); //ddeselect card
    for (int i = 0; i < 10; i++) {
        uart_send_string("isnide spi transfer loop\r\n");
        spi_transfer(0xFF); // send dummy bytes
    }
    uart_send_string("end of send dummy func\r\n");
}


/*
GPIO setup for CS pin
Dummy clocking to enter SPI mode
CMD0 send
Wait for valid response
*/
void sd_card_init(void) {
    uart_send_string("isnide cd_init func\r\n");
    // cs pin as output
    gpio_mode_setup(SPI1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI1_CS); // setting PA4 as output pin so we can control cs manually

    // send 80 dummy clocks with cs high
    uart_send_string("Sending dummy...\r\n");
    sd_send_dummy_clock();
    delay_ms(10);
    uart_send_string("Done sending dummy...\r\n");
    // send cmd0 to reset the card
    // send CMD0 - go idle state
    cs_low();
    uart_send_string("Sending CMD0(reset)...\r\n");
    spi_transfer(0x40 | 0); //cmd0
    spi_transfer(0);spi_transfer(0);spi_transfer(0);spi_transfer(0); // Arg = 0
    spi_transfer(0x95); // pre calculated CRC for CMD0
    delay_ms(10);
    /*
    0x40 | 0 → 0x40: Command 0 (with proper start bit format)
    Four zero bytes: CMD0 takes a 32-bit argument, which is 0
    0x95: Valid CRC for CMD0 (only CMD0 needs valid CRC; others don't in SPI mode)
*/

    // read response
    uint8_t r;
    for (int i = 0; i < 10; i++) {
        r = spi_transfer(0xFF); // send dummy to read
        if (r == 0x01) {
            break ;
        }
        /*
        A valid response to CMD0 is 0x01 (Idle State)
        If you get that, the SD card has entered SPI mode
        */
    }
    cs_high();

    // results
    uart_send_string("CMD0 response: 0x");
    uart_send_hex8(r);
    uart_send_string("\r\n");
}
