/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Display.h"
#include <SDL_opengl.h>

void Display_t::Clear()
{
    uint32_t len = m_SurfaceSizeX * m_SurfaceSizeY;
    
    for( uint32_t idx = 0; idx < len; idx ++ )
    {
        m_pFrame[ m_CurrentFrame ][ idx ].Red   = 0;
        m_pFrame[ m_CurrentFrame ][ idx ].Green = 0;
        m_pFrame[ m_CurrentFrame ][ idx ].Blue  = 0;
    }
}

void Display_t::Init( uint16_t xSize, uint16_t ySize )
{ 
   Init( xSize, ySize, xSize, ySize );
}

void Display_t::Init( uint16_t xSize, uint16_t ySize, uint16_t xWindowSize, uint16_t yWindowSize )
{
    m_SurfaceSizeX = xSize;
    m_SurfaceSizeY = ySize;
    m_CurrentFrame = 0;
    m_pWindow      = 0;
    m_pOpenGLContext = 0;
    /*if( m_pFrame )
    {
        delete m_pFrame;
    }*/

    m_pFrame[ 0 ] = new PixelStruct_t[ m_SurfaceSizeX * m_SurfaceSizeY ];
    m_pFrame[ 1 ] = new PixelStruct_t[ m_SurfaceSizeX * m_SurfaceSizeY ];

    SDL_Init( SDL_INIT_VIDEO );
    WindowResize( xWindowSize, yWindowSize );   
}
    

void Display_t::WindowResize( uint16_t xSize, uint16_t ySize )
{
    m_WindowSizeX  = xSize;
    m_WindowSizeY  = ySize;

    if( 0 == m_pWindow )
    {
        m_pWindow = SDL_CreateWindow("Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_WindowSizeX, m_WindowSizeY,
                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    }

    if( 0 != m_pWindow )
    {
        SDL_SetWindowSize( m_pWindow, m_WindowSizeX, m_WindowSizeY );
    }

    m_pOpenGLContext = SDL_GL_CreateContext( m_pWindow );

    glViewport(0, 0, m_WindowSizeX, m_WindowSizeY );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_WindowSizeX, m_WindowSizeY, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_SurfaceSizeX, m_SurfaceSizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Display_t::DrawPixel( uint16_t xPos, uint16_t yPos, uint8_t red, uint8_t green, uint8_t blue )
{
    if( ( xPos <= m_SurfaceSizeX ) && ( yPos <= m_SurfaceSizeY ) )
    {
        PixelStruct_t* pixel = &m_pFrame[ m_CurrentFrame ][ xPos + m_SurfaceSizeX * yPos ];

        pixel->Red   = red;
        pixel->Green = green;
        pixel->Blue  = blue;
    }
}

void Display_t::Flip( void )
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_SurfaceSizeX, m_SurfaceSizeY, GL_RGB, GL_UNSIGNED_BYTE, m_pFrame[ m_CurrentFrame ] );

    glBegin( GL_QUADS );
        glTexCoord2d(0.0, 0.0);  glVertex2d(0.0, 0.0);
        glTexCoord2d(1.0, 0.0);  glVertex2d(m_WindowSizeX, 0.0);
        glTexCoord2d(1.0, 1.0);  glVertex2d(m_WindowSizeX, m_WindowSizeY);
        glTexCoord2d(0.0, 1.0);  glVertex2d(0.0, m_WindowSizeY);
    glEnd();
 
    SDL_GL_SwapWindow( m_pWindow );

    m_CurrentFrame = !m_CurrentFrame;

}

void Display_t::SetTitle( const char* pTitle )
{
    SDL_SetWindowTitle( m_pWindow, pTitle );
}

Display_t::~Display_t()
{
    SDL_GL_DeleteContext( m_pOpenGLContext );
    SDL_DestroyWindow( m_pWindow );
    delete m_pFrame[ 0 ];
    delete m_pFrame[ 1 ];
}