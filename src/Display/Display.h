/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _Display_h_
#define _Display_h_

#include <stdint.h>
#include "SDL.h"

struct DisplayPixelStruct_t
{
    uint32_t Red   : 8;
    uint32_t Green : 8;
    uint32_t Blue  : 8;
    uint32_t Alpha : 8;
};

class Display_t
{
public:
    void Init( uint16_t xSize, uint16_t ySize, bool isVsyncEnable );
    void Init( uint16_t xSize, uint16_t ySize, uint16_t xWindowSize, uint16_t yWindowSize, bool isVsyncEnable );
    void WindowResize( uint16_t xSize, uint16_t ySize );
    void SetTitle( const char* pTitle );
    void DrawPixel( uint16_t xPos, uint16_t yPos, uint8_t red, uint8_t green, uint8_t blue );
    void Flip( void );
    void Clear();
    void* GetFrameBuffer();
    uint32_t GetSizeVertical();
    uint32_t GetSizeHorizontal();
 
    Display_t();
    ~Display_t();

private:
    SDL_Window*    m_pWindow;
    SDL_Texture *  m_Texture;
    SDL_Renderer * m_Renderer;

    uint32_t*      m_pFrame[ 2 ];   
    uint16_t       m_WindowSizeX;
    uint16_t       m_WindowSizeY;
    uint16_t       m_SurfaceSizeX;
    uint16_t       m_SurfaceSizeY;
    uint8_t        m_CurrentFrame;
};

#endif