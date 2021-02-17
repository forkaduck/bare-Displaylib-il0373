#ifndef DISPLAY_H
#define DISPLAY_H

#include <inttypes.h>
#include "io.h"

enum {
    D_PSR = 0x00,
    D_PWR = 0x01,
    D_POF = 0x02,
    D_PFS = 0x03,
    D_PON = 0x04,
    D_PMES = 0x05,
    D_BTST = 0x06,
    D_DSLP = 0x07,
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
    D_REV = 0x70,
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
    D_CCSET = 0xe0,
    D_TSSET = 0xe5
};


void send_il0373(uint8_t command, uint8_t data[], size_t datasize);
void init_il0373();
#endif
