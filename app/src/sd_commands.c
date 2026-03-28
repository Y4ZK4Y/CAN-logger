#include "sd_commands.h"
#include "spi.h"
#include "delay.h"

#include <libopencm3/stm32/gpio.h>

/* Match spi.c: CS PA4, SPI1 */
#define SD_CS_PORT GPIOA
#define SD_CS_PIN  GPIO4

static void cs_high(void)
{
	gpio_set(SD_CS_PORT, SD_CS_PIN);
}

static void cs_low(void)
{
	gpio_clear(SD_CS_PORT, SD_CS_PIN);
}

static uint8_t xchg(uint8_t b)
{
	return spi_transfer(b);
}

void sd_commands_bus_idle_clocks(unsigned n)
{
	cs_high();
	while (n--)
		xchg(0xFF);
}

static uint8_t wait_r1(unsigned max_try)
{
	uint8_t r = 0xFF;
	for (unsigned i = 0; i < max_try; i++) {
		r = xchg(0xFF);
		if ((r & 0x80u) == 0u)
			return r;
	}
	return 0xFF;
}

/* CS low: send 6-byte command, return R1, CS stays low. */
static uint8_t cmd_start_nohold(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	xchg(0xFF);
	xchg(0x40u | (cmd & 0x3Fu));
	xchg((uint8_t)(arg >> 24));
	xchg((uint8_t)(arg >> 16));
	xchg((uint8_t)(arg >> 8));
	xchg((uint8_t)arg);
	xchg(crc);
	return wait_r1(16);
}

/* Full command with CS deassert after R1 (for CMD0, CMD55, etc.). */
static uint8_t cmd_simple(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	cs_low();
	uint8_t r1 = cmd_start_nohold(cmd, arg, crc);
	cs_high();
	xchg(0xFF);
	return r1;
}

uint8_t sd_cmd0_go_idle(void)
{
	return cmd_simple(0, 0, 0x95);
}

uint8_t sd_cmd8(uint32_t check_pattern, uint8_t r7[4])
{
	cs_low();
	uint8_t r1 = cmd_start_nohold(8, check_pattern, 0x87);
	if (r7) {
		for (int i = 0; i < 4; i++)
			r7[i] = (r1 != 0xFFu) ? xchg(0xFF) : 0xFF;
	} else {
		for (int i = 0; i < 4; i++)
			xchg(0xFF);
	}
	cs_high();
	xchg(0xFF);
	return r1;
}

uint8_t sd_cmd55(void)
{
	return cmd_simple(55, 0, 0x01);
}

uint8_t sd_cmd_acmd41(uint32_t hcs_ocr_arg)
{
	return cmd_simple(41, hcs_ocr_arg, 0x01);
}

uint8_t sd_cmd58_read_ocr(uint32_t ocr[1])
{
	cs_low();
	uint8_t r1 = cmd_start_nohold(58, 0, 0x01);
	uint32_t v = 0;
	if (r1 != 0xFFu && (r1 & 0xFEu) == 0u) {
		v = (uint32_t)xchg(0xFF) << 24;
		v |= (uint32_t)xchg(0xFF) << 16;
		v |= (uint32_t)xchg(0xFF) << 8;
		v |= (uint32_t)xchg(0xFF);
	} else {
		for (int i = 0; i < 4; i++)
			xchg(0xFF);
	}
	cs_high();
	xchg(0xFF);
	if (ocr)
		ocr[0] = v;
	return r1;
}

uint8_t sd_cmd16_set_blocklen(uint32_t len)
{
	return cmd_simple(16, len, 0x01);
}

static uint8_t wait_data_token(unsigned max_u8)
{
	for (unsigned i = 0; i < max_u8; i++) {
		uint8_t t = xchg(0xFF);
		if (t == 0xFEu)
			return t;
	}
	return 0xFF;
}

uint8_t sd_cmd17_read_block(uint32_t addr, uint8_t *buf512)
{
	if (!buf512)
		return 0xFF;

	cs_low();
	uint8_t r1 = cmd_start_nohold(17, addr, 0x01);
	if (r1 != 0u) {
		cs_high();
		xchg(0xFF);
		return r1;
	}

	if (wait_data_token(65536u) != 0xFEu) {
		cs_high();
		xchg(0xFF);
		return 0xFF;
	}

	for (unsigned i = 0; i < SD_BLOCK_LEN; i++)
		buf512[i] = xchg(0xFF);
	xchg(0xFF);
	xchg(0xFF);

	cs_high();
	xchg(0xFF);
	return 0u;
}

static uint8_t wait_not_busy(unsigned max_try)
{
	for (unsigned i = 0; i < max_try; i++) {
		if (xchg(0xFF) == 0xFFu)
			return 0u;
	}
	return 0xFF;
}

uint8_t sd_cmd24_write_block(uint32_t addr, const uint8_t *buf512)
{
	if (!buf512)
		return 0xFF;

	cs_low();
	uint8_t r1 = cmd_start_nohold(24, addr, 0x01);
	if (r1 != 0u) {
		cs_high();
		xchg(0xFF);
		return r1;
	}

	xchg(0xFE);
	for (unsigned i = 0; i < SD_BLOCK_LEN; i++)
		xchg(buf512[i]);
	xchg(0xFF);
	xchg(0xFF);

	uint8_t dr = xchg(0xFF);
	if ((dr & 0x1Fu) != 0x05u) {
		cs_high();
		xchg(0xFF);
		return 0xFF;
	}

	wait_not_busy(800000u);

	cs_high();
	xchg(0xFF);
	return 0u;
}

uint32_t sd_commands_sector_address(const sd_card_info_t *info, uint32_t sector)
{
	if (!info || info->is_sdhc)
		return sector;
	return sector * SD_BLOCK_LEN;
}

bool sd_commands_card_init(sd_card_info_t *info)
{
	uint8_t r7[4];

	if (info) {
		info->ocr = 0;
		info->is_sdhc = false;
	}

	sd_commands_bus_idle_clocks(20u);

	uint8_t r1 = sd_cmd0_go_idle();
	if (r1 != 0x01u)
		return false;

	r1 = sd_cmd8(0x000001AAu, r7);
	(void)r7;
	if (r1 != 0x01u && r1 != 0x00u)
		return false;

	r1 = 0x01u;
	for (int n = 0; n < 200 && r1 != 0x00u; n++) {
		(void)sd_cmd55();
		r1 = sd_cmd_acmd41(0x40000000u);
		delay_ms(10);
	}
	if (r1 != 0x00u)
		return false;

	uint32_t ocr_word = 0;
	r1 = sd_cmd58_read_ocr(&ocr_word);
	if (r1 != 0x00u)
		return false;

	bool sdhc = (ocr_word & (1u << 30)) != 0u;
	if (info) {
		info->ocr = ocr_word;
		info->is_sdhc = sdhc;
	}

	if (!sdhc) {
		r1 = sd_cmd16_set_blocklen(SD_BLOCK_LEN);
		if (r1 != 0u)
			return false;
	}

	return true;
}
