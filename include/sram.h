#ifndef SRAM_H
#define SRAM_H

#include <stddef.h>
#include <stdint.h>

#include "io.h"

static const uint16_t SRAM_MAX = 0x1FFF;
static const uint16_t SRAM_SIZE = SRAM_MAX + 1;

// sram commands
enum {
    SRAM_READ = 0x03,
    SRAM_WRITE = 0x02,
    SRAM_RDSR = 0x05,
    SRAM_WRSR = 0x01
};

// sram access modes
enum {
    SRAM_MODE_BYTE = 0x00,
    SRAM_MODE_PAGE = 0x80,
    SRAM_MODE_SEQUENTIAL = 0x40
};

// -- basic io --
// read one byte at the given addres
uint8_t sram_read_byte(uint16_t addr);

// write one byte to the sram
void sram_write_byte(uint16_t addr, uint8_t data);

// -- status manipulation --
// read the status register of the sram
uint8_t sram_read_status();

// write to the status register
void sram_write_status(uint8_t data);

// -- sequential io --
// write an array of bytes into the sram
void sram_write_sequence(uint16_t addr, uint8_t data[], size_t size);

// read a sequence of length size into the data buffer
void sram_read_sequence(uint16_t, uint8_t *data, size_t size);

// -- convenience functions --
// fill sram with data (like memset)
void sram_set_all(uint8_t data);

// initalise sram (set sram to byte mode)
void sram_init();
#endif
