#include <inttypes.h>
#include <stdbool.h>
#include <stm32f10x.h>

#include "display.h"
#include "io.h"
#include "sram.h"


int main()
{
    size_t i;

    // run config functions
    __disable_irq();
    init_io();
    //init_display();
    __enable_irq();

    // disable hold and set mode to byte

    for (i = 0; i < 100; i++) {
        write_byte_sram(i, 0xaa);
    }

    for (i = 0; i < 100; i++) {
        if (read_byte_sram(i) == 0xaa) {
            while (1) {
                DC = !DC;
            }
        }
    }

    return 0;
}
