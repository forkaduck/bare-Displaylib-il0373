#include <stdint.h>
#include <stddef.h>

#include <stm32f10x.h>

#include "misc.h"
#include "display.h"
#include "io.h"

#include "sram.h"

inline void wait_1u(uint32_t us)
{
	uint32_t i, j;
	for (i = 0; i < us; i++) {
		for (j = 0; j < 4; j++) {
		}
	}
}

static inline void send_data_il0373(uint8_t data[], size_t datasize)
{
	size_t i;

	ECS = 0x0;

	// enable D/C (data mode)
	DC = 0x1;

	for (i = 0; i < datasize; i++) {
		spi1_send(data[i]);
	}

	while (!(SPI1->SR & SPI_SR_TXE)) {
	}
	DC = 0x0;

	spi1_reset_cs();
}

inline void send_il0373(uint8_t command, uint8_t data[], size_t datasize)
{
	// wait for display
	while (BUSY) {
	}

	ECS = 0x0;

	spi1_send(command);

	while (SPI1->SR & SPI_SR_BSY) {
	}

	if (data) {
		send_data_il0373(data, datasize);
	}

	spi1_reset_cs();
}

void init_il0373()
{
	// --- Setup the display driver ---
	// Reset and wait for display
	RST = 0x1;
	wait_1u(100);

	RST = 0x0;
	wait_1u(100);

	RST = 0x1;
	wait_1u(100);
	while (BUSY) {
	}

	{
		struct wrdata {
			uint8_t data[20];
		} wr;

		// power settings
		// Source power internal / gate power internal
		// Vcom level 0b / VGH,VGL level 00b
		// VDH 11.0V
		// VDL -11.0V
		// VDHR 4V
		wr = (struct wrdata){ .data = { 0x03, 0x00, 0x2b, 0x2b,
						0x09 } };
		send_il0373(D_PWR, wr.data, 5);

		// softstart voltage booster
		// 10ms start period a / strength 3 / min off time 6.58us
		// 10ms start period b / strength 3 / min off time 6.58us
		// 10ms start period c / strength 3 / min off time 6.58us
		wr = (struct wrdata){ .data = { 0x17, 0x17, 0x17 } };
		send_il0373(D_BTST, wr.data, 3);

		// power on
		send_il0373(D_PON, NULL, 0);

		// wait for busy
		while (BUSY) {
		}

		// setup panel settings TODO
		// Res 160x296 / LUT from reg / BW / Gate Scan up / Shift right / Booster on
		// / Soft reset on
		wr = (struct wrdata){ .data = { 0xcf } };
		send_il0373(D_PSR, wr.data, 1);

		// setup Vcom and data interval (in BW mode) TODO
		// VBD = 01b LUTWB / DDX = 11b / CDI = 0111b (10 interval)
		wr = (struct wrdata){ .data = { 0x37 } };
		send_il0373(D_CDI, wr.data, 1);

		// setup pll TODO
		// M = 111b / N = 100 -> 50Hz Frame Rate
		wr = (struct wrdata){ .data = { 0x29 } };
		send_il0373(D_PLL, wr.data, 1);

		// vcm_dc TODO
		// VCOM_DC = 0.9V
		wr = (struct wrdata){ .data = { 0x0A } };
		send_il0373(D_VDCS, wr.data, 1);

		// overwrite resolution from panel settings TODO
		// HRES = c8 (200)
		// VRES High = 0
		// VRES = c8 (200)
		wr = (struct wrdata){ .data = { D_VERTRES & 0xff,
						(D_HORZRES >> 8) & 0xff,
						D_HORZRES & 0xff } };
		send_il0373(D_TRES, wr.data, 3);

		// wait for busy
		while (BUSY) {
		}
	}
}

void push_il0373()
{
	uint8_t temp;
	size_t i;

	// send b/w frame
	send_il0373(D_DTM1, NULL, 0);
	for (i = 0; i < D_BUFF_SIZE; i++) {
		temp = sram_read_byte(i);
		send_data_il0373(&temp, 1);
	}
	send_il0373(D_DSP, NULL, 0);

	// send r/n frame
	send_il0373(D_DTM2, NULL, 0);
	for (i = 0; i < D_BUFF_SIZE; i++) {
		temp = sram_read_byte(i + D_BUFF_SIZE);
		send_data_il0373(&temp, 1);
	}
	send_il0373(D_DSP, NULL, 0);

	// send refresh
	send_il0373(D_DRF, NULL, 0);
	while (BUSY) {
	}
}

void drawpixel_il0373(uint8_t x, uint8_t y, uint8_t value)
{
	// calc bit offset into sram
	const size_t bitoffset = (size_t)(x + y * D_HORZRES);

	// calc byte offset into sram
	const size_t byteoffset = (size_t)(bitoffset / 8);
	uint8_t bwdata, rndata;

	// read byte from sram
	bwdata = sram_read_byte(byteoffset);
	rndata = sram_read_byte(byteoffset + D_BUFF_SIZE);

	{
		// calc bit which should be modified
		const uint8_t bitindex = bitoffset % 8;

		const uint8_t bit = 0x80 >> bitindex;

		// modify one bit in each byte read (bwdata/rndata)
		// and send it back
		if ((bwdata & bit) != ((value & 0x1) << (7 - bitindex))) {
			bwdata ^= bit;
			sram_write_byte(byteoffset, bwdata);
		}

		if ((rndata & bit) !=
		    (((value & 0x2) >> 1) << (7 - bitindex))) {
			rndata ^= bit;
			sram_write_byte(byteoffset + D_BUFF_SIZE, rndata);
		}
	}
}
