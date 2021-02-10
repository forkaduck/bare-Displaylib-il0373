#include <stm32f10x.h>
#include <inttypes.h>

#include "display.h"
#include "io.h"

void send_display(uint8_t command, uint8_t data[], size_t datasize)
{
    size_t i;

    // wait for display
    while (BUSY) { }

    // enable ecs pin
    ECS = 0x1;

    send_spi1(command);

    if (datasize != 0 && data != NULL) {
        // enable D/C (data mode)
        DC = 0x1;

        for (i = 0; i < datasize; i++) {
            send_spi1(data[i]);
        }
    }

    // wait for outgoing data and reset all cs
    while (SPI1->SR & SPI_SR_BSY) { }
    GPIOA->BRR = 0x100f;
}

void init_display()
{
    // --- Setup the display driver ---
    // Reset and wait for display
    while (BUSY) {}
    RST = 0x1;
    wait_10u(3);
    RST = 0x0;
    while (BUSY) { }

    {
        struct wrdata {
            uint8_t data[20];
        } wr;

        // softstart voltage booster
        // 10ms start period a / strength 3 / min off time 6.58us
        // 10ms start period b / strength 3 / min off time 6.58us
        // 10ms start period c / strength 3 / min off time 6.58us
        wr = (struct wrdata) { .data = { 0x17, 0x17, 0x17 } };
        send_display(D_BTST, wr.data, 3);

        // power settings
        // Source power internal / gate power internal
        // Vcom level 0b / VGH,VGL level 00b
        // VDH 11.0V
        // VDL -11.0V
        // VDHR 4V
        wr = (struct wrdata) { .data = { 0x03, 0x00, 0x2b, 0x2b, 0x09 } };
        send_display(D_PWR, wr.data, 5);

        // power on
        send_display(D_PON, NULL, 0);

        // wait for busy
        while(BUSY) {}

        // setup panel settings
        // Res 160x296 / LUT from reg / BW / Gate Scan up / Shift right / Booster on / Soft reset on
        wr = (struct wrdata) { .data = { 0xff} };
        send_display(D_PSR, wr.data, 1);

        // setup pll
        // M = 111b / N = 100 -> 50Hz Frame Rate
        wr = (struct wrdata) { .data = { 0x3c} };
        send_display(D_PLL, wr.data, 1);

        // overwrite resolution from panel settings
        // HRES = c8 (200)
        // VRES High = 0
        // VRES = c8 (200)
        wr = (struct wrdata) { .data = { 0xc8, 0x00, 0xc8} };
        send_display(D_TRES, wr.data, 3);

        // vcm_dc
        // VCOM_DC = 0.9V
        wr = (struct wrdata) { .data = { 0x12} };
        send_display(D_VDCS, wr.data, 1);

        // setup Vcom and data interval (in BW mode)
        // VBD = 01b LUTWB / DDX = 11b / CDI = 0111b (10 interval)
        wr = (struct wrdata) { .data = { 0x77} };
        send_display(D_CDI, wr.data, 1);
    }
}
