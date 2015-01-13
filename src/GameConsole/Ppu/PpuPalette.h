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
    uint32_t Red   : 8;
    uint32_t Green : 8;
    uint32_t Blue  : 8;
	uint32_t Alpha : 8;
};

extern RgbPixel_t* GetPalettePixel( uint8_t color );
extern uint8_t GetPalettePixel332( uint8_t color );
extern uint32_t GetPalettePixel8888( uint8_t color );
extern uint32_t* GetPalettePixel8888();
extern uint32_t* GetPalettePixelRGBA();
extern uint8_t* GetPalettePixel332();
#endif