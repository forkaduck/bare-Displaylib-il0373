#include <stdint.h>

#include "stm32f10x.h"

#include "debug.h"
#include "io.h"
#include "sram.h"

static inline void sram_sendaddr(uint16_t addr)
{
	// discard anything over the max
	addr &= SRAM_MAX;

	// send address in two byte fractions
	spi1_send((addr & 0xff00) >> 8);
	spi1_send(addr & 0x00ff);
}

// --- basic io ---
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

void sram_write_byte(uint16_t addr, uint8_t data)
{
	SRCS = 0x0;

	spi1_send(SRAM_WRITE);
	sram_sendaddr(addr);
	spi1_send(data);

	spi1_reset_cs();
}

// --- status manipulation ---
uint8_t sram_read_status()
{
	uint8_t temp;
	SRCS = 0x0;

	spi1_send(SRAM_RDSR);
	temp = spi1_rec();

	spi1_reset_cs();
	return temp & 0xf0;
}

void sram_write_status(uint8_t data)
{
	SRCS = 0x0;

	spi1_send(SRAM_WRSR);
	spi1_send(data | 0x3);

	spi1_reset_cs();
}

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
void sram_set_all(uint8_t data)
{
	size_t i;
	for (i = 0; i < SRAM_SIZE; i++) {
		sram_write_byte(i, data);
	}
}

void sram_init()
{
	sram_write_status(SRAM_MODE_BYTE);
}
