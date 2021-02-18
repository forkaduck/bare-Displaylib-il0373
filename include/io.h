#ifndef IO_H
#define IO_H

#include <inttypes.h>

// bit_word_addr = bit_band_base + (byte_offset * 32) + (bit_number * 4)
#define BB_PERIPH(byteoff, bitnum) (PERIPH_BB_BASE + (byteoff - PERIPH_BASE) * 32 + (bitnum * 4))

#define ECS *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 3 * sizeof(uint32_t), 0)))
#define DC *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 3 * sizeof(uint32_t), 1)))
#define SRCS *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 3 * sizeof(uint32_t), 12)))
#define SDCS *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 3 * sizeof(uint32_t), 3)))

#define RST *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 3 * sizeof(uint32_t), 8)))
#define BUSY *((volatile uint32_t*)(BB_PERIPH(GPIOA_BASE + 2 * sizeof(uint32_t), 11)))


#define size_t uint64_t
#define NULL ((void*)0)

enum {
    spi1_fpclk2 = 0x0,
    spi1_fpclk4 = 0x1,
    spi1_fpclk8 = 0x2,
    spi1_fpclk16 = 0x3,
    spi1_fpclk32 = 0x4,
    spi1_fpclk64 = 0x5,
    spi1_fpclk128 = 0x6,
    spi1_fpclk256 = 0x7
};

void spi1_init();
void spi1_reset_cs();
void spi1_send(uint8_t data);
uint8_t spi1_rec();
void wait_1u(uint32_t us);

#endif
