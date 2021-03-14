#include <stdint.h>
#include <stddef.h>

#include "stm32f10x.h"

#include "misc.h"
#include "io.h"
#include "sram.h"

// splits and sends the address to the display
static inline void sram_sendaddr(uint16_t addr)
{
	// discard anything over the max
	addr &= SRAM_MAX;

	// send address in two byte fractions
	spi1_send((addr & 0xff00) >> 8);
	spi1_send(addr & 0x00ff);
}

// --- basic io ---
// returns one byte read at the location given
uint8_t sram_read_byte(uint16_t addr)
{
	uint8_t temp = 0x00;

	SRCS = 0x0;

	// send read followed by the address
	spi1_send(SRAM_READ);
	sram_sendaddr(addr);

	// wait for data from sram
	temp = spi1_rec();

	spi1_reset_cs();
	return temp;
}

// writes one byte to the given memory location
void sram_write_byte(uint16_t addr, uint8_t data)
{
	SRCS = 0x0;

	spi1_send(SRAM_WRITE);
	sram_sendaddr(addr);
	spi1_send(data);

	spi1_reset_cs();
}

// --- status manipulation ---
// returns the content of the status register
uint8_t sram_read_status()
{
	uint8_t temp;
	SRCS = 0x0;

	spi1_send(SRAM_RDSR);
	temp = spi1_rec();

	spi1_reset_cs();
	return temp & 0xf0;
}

// write to the status register
void sram_write_status(uint8_t data)
{
	SRCS = 0x0;

	spi1_send(SRAM_WRSR);
	spi1_send(data | 0x3);

	spi1_reset_cs();
}

// write an array of bytes into the sram at a given location
void sram_write_sequence(uint16_t addr, uint8_t data[], size_t size)
{
	size_t i;
	assert(size <= SRAM_SIZE);

	// change mode to sequential
	sram_write_status(SRAM_MODE_SEQUENTIAL);
	assert(sram_read_status() == SRAM_MODE_SEQUENTIAL);

	SRCS = 0x0;

	// issue write command and addr to sram
	spi1_send(SRAM_WRITE);
	sram_sendaddr(addr);

	for (i = 0; i < size; i++) {
		spi1_send(data[i]);
	}

	spi1_reset_cs();

	// reset mode to byte wise
	sram_write_status(SRAM_MODE_BYTE);
	assert(sram_read_status() == SRAM_MODE_BYTE);
}

// write the read bytes into a preallocated array from the given address
void sram_read_sequence(uint16_t addr, uint8_t *data, size_t size)
{
	size_t i;
	assert(size <= SRAM_SIZE);

	// change mode to sequential
	sram_write_status(SRAM_MODE_SEQUENTIAL);
	assert(sram_read_status() == SRAM_MODE_SEQUENTIAL);

	SRCS = 0x0;

	spi1_send(SRAM_READ);
	sram_sendaddr(addr);

	for (i = 0; i < size; i++) {
		data[i] = spi1_rec();
	}

	spi1_reset_cs();

	// reset mode to byte wise
	sram_write_status(SRAM_MODE_BYTE);
	assert(sram_read_status() == SRAM_MODE_BYTE);
}

// --- convenience functions ---
// sets the array to the given byte (like memset)
void sram_set_all(uint8_t data)
{
	size_t i;
	for (i = 0; i < SRAM_SIZE; i++) {
		sram_write_byte(i, data);
	}
}

// sets the into byte mode
// (the reset state of the control register should
// be in byte mode but just to be safe)
void sram_init()
{
	sram_write_status(SRAM_MODE_BYTE);
}
