#ifndef IO_H
#define IO_H

#include <stdint.h>

// bit_word_addr = bit_band_base + (byte_offset * 32) + (bit_number * 4)
#define BB_PERIPH(byteoff, bitnum) (PERIPH_BB_BASE + (byteoff - PERIPH_BASE) * 32 + (bitnum * 4))

#define ECS *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 3 * sizeof(uint32_t), 0)))
#define DC *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 3 * sizeof(uint32_t), 1)))
#define SRCS *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 3 * sizeof(uint32_t), 12)))
#define SDCS *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 3 * sizeof(uint32_t), 3)))

#define RST *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 3 * sizeof(uint32_t), 8)))
#define BUSY *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 2 * sizeof(uint32_t), 11)))

void spi1_init();
void spi1_reset_cs();
void spi1_send(uint8_t data);
uint8_t spi1_rec();

#endif
