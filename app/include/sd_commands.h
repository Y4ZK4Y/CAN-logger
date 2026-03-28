#ifndef SD_COMMANDS_H
#define SD_COMMANDS_H

#include <stdbool.h>
#include <stdint.h>

/* SPI SD command helpers (SPI mode, manual CS on PA4).
 * Uses your existing spi_transfer() / SPI1 wiring from spi.c.
 * Independent of sd.c / sd_card_init(). */

#define SD_BLOCK_LEN 512u

typedef struct {
	uint32_t ocr;
	bool     is_sdhc; /* CCS from OCR (CMD58) */
} sd_card_info_t;

/* Idle bus: CS high, clock out n× 0xFF (e.g. n >= 10 before CMD0). */
void sd_commands_bus_idle_clocks(unsigned n);

/* --- Individual commands (SPI); R1 is 0xFF if no valid response byte. --- */
uint8_t sd_cmd0_go_idle(void);
/* CMD8: check pattern typically 0x1AA; fills r7[4] when R1 bit7 cleared. */
uint8_t sd_cmd8(uint32_t check_pattern, uint8_t r7[4]);
uint8_t sd_cmd55(void);
uint8_t sd_cmd58_read_ocr(uint32_t ocr[1]);
uint8_t sd_cmd16_set_blocklen(uint32_t len);
uint8_t sd_cmd_acmd41(uint32_t hcs_ocr_arg);

/* Data transfer: pass address as required by card (byte addr SDSC, sector SDHC). */
uint8_t sd_cmd17_read_block(uint32_t addr, uint8_t *buf512);
uint8_t sd_cmd24_write_block(uint32_t addr, const uint8_t *buf512);

/* Sector → address for CMD17/CMD24 (uses is_sdhc from info). */
uint32_t sd_commands_sector_address(const sd_card_info_t *info, uint32_t sector);

/* Full init: idle clocks, CMD0, CMD8, ACMD41 loop, CMD58, CMD16 if SDSC.
 * Returns true on success; *info filled. */
bool sd_commands_card_init(sd_card_info_t *info);

#endif
