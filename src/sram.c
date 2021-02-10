#include <inttypes.h>
#include "stm32f10x.h"

#include "io.h"

uint8_t read_byte_sram(uint16_t addr)
{
    uint8_t temp;

    SRCS = 0x1;

    // send read instruction and address
    send_spi1(0x03);
    send_spi1((addr & 0xff00) >> 8);
    send_spi1(addr & 0x00ff);

    temp = rec_spi1();

    // wait for spi1 to finish
    while (SPI1->SR & SPI_SR_BSY) { }
    GPIOA->BRR = 0x100f;
    return temp;
}

void write_byte_sram(uint16_t addr, uint8_t data)
{
    SRCS = 0x1;

    // send write and address
    send_spi1(0x02);
    send_spi1((addr & 0xff00) >> 8);
    send_spi1(addr & 0x00ff);

    // write byte
    send_spi1(data);

    // wait for spi to finish
    while (SPI1->SR & SPI_SR_BSY) { }
    GPIOA->BRR = 0x100f;
}

uint8_t read_status_sram()
{
    uint8_t temp;
    SRCS = 0x1;

    // send read status
    send_spi1(0x05);

    temp = rec_spi1();

    // wait for spi1 to finish
    while (SPI1->SR & SPI_SR_BSY) { }
    GPIOA->BRR = 0x100f;
    return temp;
}

void write_status_sram(uint8_t data)
{
    SRCS = 0x1;

    // send write status and data
    send_spi1(0x01);
    send_spi1(data);

    // wait for spi1 to finish
    while (SPI1->SR & SPI_SR_BSY) { }
    GPIOA->BRR = 0x100f;
}

