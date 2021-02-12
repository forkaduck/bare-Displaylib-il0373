#include <inttypes.h>
#include "stm32f10x.h"

#include "io.h"
#include "sram.h"
#include "debug.h"

static inline void send_addr(uint16_t addr) {
    send_spi1((addr & 0xff00) >> 8);
    send_spi1(addr & 0x00ff);
}


// --- basic io ---
uint8_t sram_read_byte(uint16_t addr)
{
    uint8_t temp;

    SRCS = 0x0;

    send_spi1(SRAM_READ);
    send_addr(addr);

    temp = rec_spi1();

    // wait for spi1 to finish
    while (SPI1->SR & SPI_SR_BSY) { }
    RESET_CS;
    return temp;
}

void sram_write_byte(uint16_t addr, uint8_t data)
{
    SRCS = 0x0;

    send_spi1(SRAM_WRITE);
    send_addr(addr);

    // write byte
    send_spi1(data);

    // wait for spi to finish
    while (SPI1->SR & SPI_SR_BSY) { }
    RESET_CS;
}


// --- status manipulation ---
uint8_t sram_read_status()
{
    uint8_t temp;
    SRCS = 0x0;

    // send read status
    send_spi1(SRAM_RDSR);

    temp = rec_spi1();

    // wait for spi1 to finish
    while (SPI1->SR & SPI_SR_BSY) { }
    RESET_CS;
    return temp;
}

void sram_write_status(uint8_t data)
{
    SRCS = 0x0;

    // send write status and data
    send_spi1(SRAM_WRSR);
    send_spi1(data);

    // wait for spi1 to finish
    while (SPI1->SR & SPI_SR_BSY) { }
    RESET_CS;
}

void sram_write_sequence(uint16_t addr, uint8_t data[], size_t size) {
    size_t i = 0;

    assert(size <= SRAM_SIZE);

    // change mode to sequential
    sram_write_status(SRAM_MODE_SEQUENTIAL);

    assert(sram_read_status() == (SRAM_MODE_SEQUENTIAL | 0x2));

    SRCS = 0x0;

    // issue write command and addr to sram
    send_spi1(SRAM_WRITE);
    send_addr(addr);

    for(i = 0; i < size; i++) {
        send_spi1(data[i]);
    }

    RESET_CS;

    // reset mode to byte wise
    sram_write_status(SRAM_MODE_BYTE);

    assert(sram_read_status() == (SRAM_MODE_BYTE | 0x2));
}

// --- convenience functions ---
void sram_clear() {
    size_t i;
    uint8_t data[SRAM_SIZE];

    for(i = 0; i < SRAM_SIZE; i++) {
        data[i] = 0x00;
    }

    sram_write_sequence(0x0000, data, SRAM_SIZE);
}

void sram_init() {
    sram_write_status(SRAM_MODE_BYTE);
}
