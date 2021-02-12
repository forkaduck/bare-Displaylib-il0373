
#include <stm32f10x.h>
#include <inttypes.h>

#include "io.h"
#include "debug.h"

#ifdef DEBUG
#define SPI_SPEED_SELECT SPI_CR1_BR_1
#else
#define SPI_SPEED_SELECT 0
#endif


void init_io() {
    // enable spi1 and port a
    RCC->APB2ENR = RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;

    // configure port a per readme
    GPIOA->CRL = 0xb4bb3333;
    GPIOA->CRH = 0x44433443;

    // reset all cs
    RESET_CS;

    // configure spi 8b / master / (fpck/2)
    SPI1->CR1 = SPI_CR1_MSTR | SPI_SPEED_SELECT;

    SPI1->CR1 |= SPI_CR1_SPE;
}

void send_spi1(uint8_t data) {
    // wait for spi hardware
    while(SPI1->SR & SPI_SR_BSY) {}
    SPI1->DR = data;
}

uint8_t rec_spi1() {
    // send 0x00 to generate clock for answer
    send_spi1(0x00);

    // wait for input in shift register
    while(!(SPI1->SR & SPI_SR_RXNE)) {}

    return SPI1->DR;
}

inline void wait_1u(uint32_t us) {
    uint8_t i;
    for(i = 0; i < 4; i++) {
    }
}
