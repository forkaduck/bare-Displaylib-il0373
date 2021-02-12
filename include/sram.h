#ifndef SRAM_H
#define SRAM_H

#include <inttypes.h>

#define SRAM_SIZE 64

enum {
    SRAM_READ = 0x03,
    SRAM_WRITE = 0x02,
    SRAM_RDSR = 0x05,
    SRAM_WRSR = 0x01
};

enum {
    SRAM_MODE_BYTE = 0x01,
    SRAM_MODE_PAGE = 0x81,
    SRAM_MODE_SEQUENTIAL = 0x41
};

uint8_t sram_read_byte(uint16_t addr);
void sram_write_byte(uint16_t addr, uint8_t data);

uint8_t sram_read_status();
void sram_write_status(uint8_t data);

void sram_clear();
void sram_init();
#endif
