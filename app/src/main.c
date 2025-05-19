#include "common_defines.h"
#include "uart.h"
//#include "system.h"
#include "can.h"
#include "sd.h"
#include "logger.h"
#include <libopencm3/stm32/can.h>
#include "uart.h"


// int main(void) {
//     //system_init();
//     //clock_setup();
//     uart_setup();
    
//     // can_init();
//     // sd_init();
//     // logger_init();

//     while (1) {
//         uart_send_string("CAN setup done.\r\n");
//         for (volatile int i = 0; i < 800000; i++);
//         // can_frame_t frame;
//         // if (can_receive(&frame)) {
//         //     logger_log_frame(&frame);
//         // }
//     }

// }

int main(void)
{
    uart_setup();
    can_setup();

    uint32_t id;
    bool ext, rtr;
    uint8_t fmi, length;
    uint8_t data[8];

    while (1) {
        can_receive(CAN1, 0, true, &id, &ext, &rtr, &fmi, &length, data, NULL);

        uart_send_string("\nCAN Frame: ID=0x");
        uart_send_hex32(id);
        uart_send_string(" DLC=");
        uart_send_uint(length);

        uart_send_string(" Data:");
        for (uint8_t i = 0; i < length; i++) {
            uart_send_string(" ");
            uart_send_hex8(data[i]);
        }

        uart_send_string("\n");
    }
}


