#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "stm32f10x.h"

#include "misc.h"
#include "display.h"
#include "io.h"
#include "sram.h"
#include "graph.h"

// --- Tests ---
// SRAM - Working
// 0 - Reading and writing
// 1 - Mode write and read
// 2 - Sequential writing
// 3 - Sequential reading

// Display - Working
// 4 init && direct write to display
// 5 init && sram as framebuffer
// 6 init && sram && drawpixel routine

void test(uint8_t testnr)
{
	size_t i;

	switch (testnr) {
	case 0:
		// check if reading and writing one byte is working
		for (i = 0; i < 0xff; i++) {
			sram_write_byte(0x0000, i);
			assert(sram_read_byte(0x0000) == i);
		}
		break;

	case 1:
		// check if read and write to status has effect
		sram_write_status(SRAM_MODE_SEQUENTIAL);
		assert(sram_read_status() == SRAM_MODE_SEQUENTIAL);
		sram_write_status(SRAM_MODE_BYTE);
		break;

	case 2:
		// check if sram_clear resets all data
		{
			sram_set_all(0xff);

			for (i = 0; i < SRAM_SIZE; i++) {
				assert(sram_read_byte(i) == 0x00);
			}
		}
		break;

	case 3: {
		// sequential read and write test
		uint8_t j = 0;
		uint8_t data[SRAM_SIZE];
		uint8_t returndat[SRAM_SIZE];

		// set data to a recognisable pattern
		for (i = 0; i < SRAM_SIZE; i++) {
			data[i] = j;
			j++;
		}

		sram_write_sequence(0x0000, data, SRAM_SIZE);

		sram_read_sequence(0x0000, returndat, SRAM_SIZE);

		// check if the same pattern was read
		for (i = 0; i < SRAM_SIZE; i++) {
			assert(data[i] == returndat[i]);
		}

	} break;

	case 4:
		// push zebra pattern with shift to display directly
		{
			uint8_t framebuffer[D_BUFF_SIZE];

			for (i = 0; i < D_BUFF_SIZE / 2; i++) {
				framebuffer[i] = 0x33;
			}

			for (i = D_BUFF_SIZE / 2; i < D_BUFF_SIZE; i++) {
				framebuffer[i] = 0xcc;
			}

			// send frame
			send_il0373(D_DTM1, framebuffer, D_BUFF_SIZE);
			send_il0373(D_DSP, NULL, 0);

			for (i = 0; i < D_BUFF_SIZE; i++) {
				framebuffer[i] = 0xff;
			}

			send_il0373(D_DTM2, framebuffer, D_BUFF_SIZE);
			send_il0373(D_DSP, NULL, 0);

			// send refresh
			send_il0373(D_DRF, NULL, 0);
		}
		break;

	case 5:
		// write to sram and push the content to the display
		// should show a zebra like pattern
		{
			uint8_t framebuffer[D_BUFF_SIZE];

			sram_set_all(0xff);

			// set b/w buffer to zebra pattern which splits in the middle
			for (i = 0; i < D_BUFF_SIZE / 2; i++) {
				framebuffer[i] = 0xcc;
			}

			for (i = D_BUFF_SIZE / 2; i < D_BUFF_SIZE; i++) {
				framebuffer[i] = 0x33;
			}

			sram_write_sequence(0x0000, framebuffer, D_BUFF_SIZE);

			// set r/n buffer to show red on one of the half
			for (i = 0; i < D_BUFF_SIZE / 2; i++) {
				framebuffer[i] = 0x33;
			}

			sram_write_sequence(D_BUFF_SIZE, framebuffer,
					    D_BUFF_SIZE);

			push_il0373();
		}
		break;

	case 6:
		// test if the drawpixel function works
		// by drawing some random lines to test overlapping
		{
			sram_set_all(0xff);

			// draw vertical line and horizontal lines
			for (i = 0; i < 100; i++) {
				drawpixel_il0373(8, i + 8, D_BLACK | D_NONE);

				drawpixel_il0373(i + 8, 20, D_BLACK | D_NONE);
				drawpixel_il0373(i + 8, 21, D_WHITE | D_RED);
			}

			// draw horizontal line which is waved verticaly
			for (i = 0; i < 100; i++) {
				if (i % 2 == 0) {
					drawpixel_il0373(i + 8, 8,
							 D_WHITE | D_RED);
				} else {
					drawpixel_il0373(i + 8, 9,
							 D_BLACK | D_NONE);
				}
			}

			// draw one line with an angle of 45 degree
			for (i = 0; i < 100; i++) {
				drawpixel_il0373(i + 8, i + 8, D_WHITE | D_RED);
			}

			push_il0373();
		}
		break;

	case 7:
		// test drawline
		{
			sram_set_all(0xff);

			drawline(2, 2, D_HORZRES - 2, D_VERTRES - 2,
				 D_BLACK | D_NONE);

			drawline(5, 100, 10, 2, D_WHITE | D_RED);
			push_il0373();
		}
	}
}

int main()
{
	// run config functions
	__disable_irq();
	spi1_init();
	sram_init();
	init_il0373();
	__enable_irq();

	test(7);
	return 0;
}
