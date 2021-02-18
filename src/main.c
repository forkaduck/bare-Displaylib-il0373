#include <inttypes.h>
#include <stdbool.h>
#include <stm32f10x.h>

#include "debug.h"
#include "display.h"
#include "io.h"
#include "sram.h"

// Display - Working
// broken - init
// broken - sending commands

// SRAM - Working
// 0 - Reading and writing
// 1 - Mode write and read
// 2 - Sequential writing
// 3 - Sequential reading

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
			sram_clear();

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
		// push completely black frame to display directly
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
		// result: all black display, if white is shown readjust resolution
		{
			uint8_t framebuffer[D_BUFF_SIZE];

			sram_clear();

			for (i = 0; i < D_BUFF_SIZE; i++) {
				framebuffer[i] = 0x00;
			}

			sram_write_sequence(0x0000, framebuffer, D_BUFF_SIZE);

			for (i = 0; i < D_BUFF_SIZE; i++) {
				framebuffer[i] = 0xff;
			}

			sram_write_sequence(D_BUFF_SIZE, framebuffer,
					    D_BUFF_SIZE);

			push_il0373();
			break;
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

	test(4);
	return 0;
}
