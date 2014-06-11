/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _PpuPalette_h_
#define _PpuPalette_h_

#include <stdint.h>

struct RgbPixel_t
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};

extern RgbPixel_t* GetPalettePixel( uint8_t color );


#endif