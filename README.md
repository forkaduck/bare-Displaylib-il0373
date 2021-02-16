# Displaylib
## Pinning
#### PA15..0:
###### Display pins
PA0: ECS
PA1: D/C
PA2: (DEAD ON NUCLEO BOARD)SRCS
PA3: SDCS

###### SPI
PA4: SPI1_NSS
PA5: SPI1_SCK
PA6: SPI1_MISO
PA7: SPI1_MOSI

###### Display pins
PA8: RST

###### UART
PA9: TX
PA10: RX

###### Display pins
PA11: BUSY
PA12: new SRCS

PA13...15: Reserved


## Display
* 14 MHz theoretical speed
* update time of 180 seconds (27s full refresh time)
* 3.3/5V
* MSB first
* CPOL ??? (0 for now)
* CPHA ??? (1 clock transistion for now)

## SRAM
* 20 MHz theoretical speed
* MSB first
