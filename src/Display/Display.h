/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _Display_h_
#define _Display_h_

#include <stdint.h>
#include "SDL.h"

struct PixelStruct_t
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};

class Display_t
{
public:
    void Init( uint16_t xSize, uint16_t ySize );
    void Init( uint16_t xSize, uint16_t ySize, uint16_t xWindowSize, uint16_t yWindowSize );
    void WindowResize( uint16_t xSize, uint16_t ySize );
    void SetTitle( const char* pTitle );
    void Present( PixelStruct_t* pData );
    void DrawPixel( uint16_t xPos, uint16_t yPos, uint8_t red, uint8_t green, uint8_t blue );
    void Flip( void );
    void Clear();
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