
#include <stm32f10x.h>

#include "io.h"

void init_io()
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;

    // configure SPI / SDCS / old SRCS / DC / ECS
    GPIOA->CRL = 0xb4bb3333;

    // configure new SRCS / Busy / RST
    GPIOA->CRH = 0x00034003;

    // configure SPI1 to be full duplex, master, with fsck / 8
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_1;
    SPI1->CR1 |= SPI_CR1_SPE;
}

inline void send_spi1(uint8_t data)
{
    // check and wait if spi hardware needs some more time
    while (SPI1->SR & SPI_SR_BSY) { }
    SPI1->DR = data;
}

inline uint8_t rec_spi1()
{
    // wait for hardware and data in shift register
    while(SPI1->SR & SPI_SR_BSY) {}
    return SPI1->DR;
}

inline void wait_10u(int us)
{
    uint16_t i;
    uint8_t j = 1, k = 2;

    for (i = 0; i < us; i++) {
        j ^= k;
        k ^= j;
        j ^= k;
    }
}
