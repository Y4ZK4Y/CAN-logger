#include "can.h"
#include <libopencm3/stm32/can.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

void can_setup(void) {
    // enable clocks
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_CAN1); // can1 or can2 ?

    // configure can pins: PA11 RX - PA12 TX
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11 | GPIO12);
    gpio_set_af(GPIOA, GPIO_AF9, GPIO11 | GPIO12);

    // init CAN
    can_reset(CAN1);
    can_init(CAN1, // can port
            false, //ttcm
            true, //abom
            false, //awum
            true, //nart
            false, //rflm
            false, //txfp
            CAN_BTR_SJW_1TQ, //sjw - 1
            CAN_BTR_TS1_8TQ, //ts1 - 6
            CAN_BTR_TS2_1TQ, // ts2 - 1
            12, // brp (prescaler) - 12 - (adjust for 500kbps @ 48 MHZ)
            false, // loopback
            true // silent
    );

    // enable CAN
    //can_enable(CAN1);

    // set filter to accept all
    can_filter_id_mask_32bit_init(0, 0, 0, 0, true);
}


