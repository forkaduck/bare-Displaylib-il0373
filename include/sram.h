#ifndef SRAM_H
#define SRAM_H

#include <inttypes.h>

uint8_t read_byte_sram(uint16_t addr);
void write_byte_sram(uint16_t addr, uint8_t data);

uint8_t read_status_sram();
void write_status_sram(uint8_t data);
#endif
