#ifndef SRAM_H
#define SRAM_H

#include <stddef.h>
#include <stdint.h>

#include "io.h"

static const uint16_t SRAM_MAX = 0x1FFF;
static const uint16_t SRAM_SIZE = SRAM_MAX + 1;

enum {
    SRAM_READ = 0x03,
    SRAM_WRITE = 0x02,
    SRAM_RDSR = 0x05,
    SRAM_WRSR = 0x01
};

enum {
    SRAM_MODE_BYTE = 0x00,
    SRAM_MODE_PAGE = 0x80,
    SRAM_MODE_SEQUENTIAL = 0x40
};

uint8_t sram_read_byte(uint16_t addr);
void sram_write_byte(uint16_t addr, uint8_t data);

uint8_t sram_read_status();
void sram_write_status(uint8_t data);

void sram_write_sequence(uint16_t addr, uint8_t data[], size_t size);
void sram_read_sequence(uint16_t, uint8_t *data, size_t size);

void sram_set_all(uint8_t data);
void sram_init();
#endif
