#include "system.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>

void clock_setup(void)
{
    // Enable HSI16 (16 MHz internal oscillator)
    rcc_osc_on(RCC_HSI16);
    rcc_wait_for_osc_ready(RCC_HSI16);

    // Set flash latency (wait states) for 80MHz
    flash_set_ws(4);

    // Configure and enable PLL: 
    // 16 MHz HSI / 4 * 40 / 2 = 80 MHz
    rcc_set_main_pll(RCC_PLLCFGR_PLLSRC_HSI16, 4, 40, 0, 0, RCC_PLLCFGR_PLLR_DIV2);
    rcc_osc_on(RCC_PLL);
    rcc_wait_for_osc_ready(RCC_PLL);

    // Select PLL as system clock source
    rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
    rcc_wait_for_sysclk_status(RCC_PLL);

    // Set clock variables (used by libopencm3)
    rcc_ahb_frequency = 80e6;
    rcc_apb1_frequency = 80e6;
    rcc_apb2_frequency = 80e6;
}
