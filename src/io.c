
#include <stdint.h>
#include <stm32f10x.h>

#include "misc.h"
#include "io.h"

void spi1_init()
{
	// enable spi1 and port a
	RCC->APB2ENR = RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;

	// configure port a per readme
	GPIOA->CRL = 0xb8bb3333;
	GPIOA->CRH = 0x88833883;
	GPIOA->ODR = 0x00000000;

	// configure spi 8b / master / (fpck/4)
	SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_1;
	SPI1->CR2 = 0x0000;

	SPI1->SR = 0x0002; // reset status register

	SPI1->CR1 |= SPI_CR1_SPE;
	while (SPI1->SR & SPI_SR_BSY) {
	}
}

inline void spi1_reset_cs()
{
	while (SPI1->SR & SPI_SR_BSY) {
	}
	GPIOA->BSRR = 0x100d;
}

inline void spi1_send(uint8_t data)
{
	// wait for tx spi hardware
	SPI1->DR = data;
	while (!(SPI1->SR & SPI_SR_TXE)) {
	}
}

inline uint8_t spi1_rec()
{
	// take out the trash
	while (SPI1->SR & SPI_SR_RXNE) {
		// just use the DR register in some way
		// to unset the flag
		if (SPI1->DR == 0x00) {
			SPI1->SR &= ~SPI_SR_OVR;
		}
	}

	// send one extra byte to generate clock for return val
	SPI1->DR = 0x00;
	while (!(SPI1->SR & SPI_SR_TXE)) {
	}

	// wait for input in shift register
	while (!(SPI1->SR & SPI_SR_RXNE)) {
	}
	return SPI1->DR;
}
