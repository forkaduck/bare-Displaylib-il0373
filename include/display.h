#ifndef DISPLAY_H
#define DISPLAY_H

#include <inttypes.h>
#include "io.h"

#define verticalRes 200
#define horizontalRes 200

enum {
    D_PSR = 0x0,
    D_PWR = 0x1,
    D_POFF = 0x2,
    D_PFS = 0x3,
    D_PON = 0x4,
    D_PMES = 0x5,
    D_BTST = 0x6,
    D_DSLP = 0x7,
    D_DTM1 = 0x10,
    D_DSP = 0x11,
    D_DRF = 0x12,
    D_DTM2 = 0x13,
    D_LUTC = 0x20,
    D_LUTWW = 0x21,
    D_LUTBWR = 0x22,
    D_LUTWBW = 0x23,
    D_LUTBBB = 0x24,
    D_PLL = 0x30,
    D_TSC = 0x40,
    D_TSE = 0x41,
    D_TSW = 0x42,
    D_TSR = 0x43,
    D_CDI = 0x50,
    D_LPD = 0x51,
    D_TCON = 0x60,
    D_TRES = 0x61,
    D_FLG = 0x71,
    D_AMVCOM = 0x80,
    D_RVCOM = 0x81,
    D_VDCS = 0x82,
    D_PTL = 0x90,
    D_PTIN = 0x91,
    D_PTOUT = 0x92,
    D_PGM = 0xa0,
    D_APG = 0xa1,
    D_ROTP = 0xa2,
    D_PWS = 0xe3
};


void send_display(uint8_t command, uint8_t data[], size_t datasize);
void init_display();
#endif
