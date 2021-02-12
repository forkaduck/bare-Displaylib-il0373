#include <inttypes.h>
#include <stdbool.h>
#include <stm32f10x.h>

#include "display.h"
#include "io.h"
#include "sram.h"

// SRAM - Working
// Reading and writing
// Sequential writing - broken again

void test() {
    // check if reading and writing one byte is working
    /*while(sram_read_byte(0x0000) != 0xaa) {
        wait_1u(1000000);
        sram_write_byte(0x0000, 0xaa);
    }*/

    // check if sram_clear resets all data
    /*sram_clear();

    uint16_t i;
    bool stop = false;
    while (!stop) {
        stop = true;

        for (i = 0; i < 65535; i++) {
            if (sram_read_byte(i) != 0x00) {
                stop = false;
            }
        }
    }*/

    // check if read and write to status has effect
    while(sram_read_status() != (SRAM_MODE_SEQUENTIAL | 0x2)) {
        sram_write_status(SRAM_MODE_SEQUENTIAL);
    }
    sram_write_status(SRAM_MODE_BYTE);
}


int main()
{
    // run config functions
    __disable_irq();
    init_io();
    sram_init();
    //init_display();
    __enable_irq();

    /*{
        const size_t buffersize = (200 * 200) / 8;
        uint8_t framebuffer[buffersize];

        // send frame
        send_display(D_DTM1, framebuffer, buffersize);

        // send data stop
        send_display(D_DSP, NULL, 0);

        // send refresh
        send_display(D_DRF, NULL, 0);
    }*/

    test();

    SRCS = 0x0;
    SRCS = 0x1;

    return 0;
}
