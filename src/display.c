#include <inttypes.h>
#include <stm32f10x.h>

#include "display.h"
#include "io.h"

#include "sram.h"

void send_il0373(uint8_t command, uint8_t data[], size_t datasize)
{
	size_t i;

	// wait for display
	while (BUSY) {
	}

	ECS = 0x0;

	spi1_send(command);

	while (SPI1->SR & SPI_SR_BSY) {
	}

	if (data) {
		// enable D/C (data mode)
		DC = 0x1;

		for (i = 0; i < datasize; i++) {
			spi1_send(data[i]);
		}
		DC = 0x0;
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

		// overwrite resolution from panel settings
		// HRES = c8 (200)
		// VRES High = 0
		// VRES = c8 (200)
		wr = (struct wrdata){ .data = { 0xc8, 0x00, 0xc8 } };
		send_il0373(D_TRES, wr.data, 3);

		// wait for busy
		while (BUSY) {
		}
	}
}

void push_il0373()
{
	uint8_t framebuffer[D_BUFF_SIZE];

	// read framebuffer from sram (b/w)
	sram_read_sequence(0x0000, framebuffer, D_BUFF_SIZE);

	// send b/w frame
	send_il0373(D_DTM1, framebuffer, D_BUFF_SIZE);
	send_il0373(D_DSP, NULL, 0);

	// read framebuffer from sram (r/n)
	sram_read_sequence(D_BUFF_SIZE, framebuffer, D_BUFF_SIZE);

	// send r/n
	send_il0373(D_DTM2, framebuffer, D_BUFF_SIZE);
	send_il0373(D_DSP, NULL, 0);

	// send refresh
	send_il0373(D_DRF, NULL, 0);
}

void drawpixel_il0373(uint8_t x, uint8_t y, uint8_t value)
{
	const size_t bitoffset = (size_t)(x + y * D_HORZRES);
	uint8_t bwdata, rndata;
	uint16_t addr = bitoffset / 8;

	// read byte from sram
	bwdata = sram_read_byte(addr);
	rndata = sram_read_byte(addr + SRAM_SIZE);

	{
		const uint8_t bitmask = ~(0x1 << (bitoffset % 8));

		sram_write_byte(addr, (bwdata & bitmask) | (value & 0x1));
		sram_write_byte(addr + SRAM_SIZE,
				(rndata & bitmask) | ((value & 0x2) >> 1));
	}
}
