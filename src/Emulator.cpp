/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Emulator.h"
#include <stdio.h>

void Emulator_t::Init()
{
    SDL_Init( SDL_INIT_TIMER );

    m_GameConsoleStarted = false;

    m_GameConsole.SetPresentFrameCallBack( PresentFrame, this );
    m_GameConsole.SetRomFileAccesCallBack( RomFileAcces, this );
    m_GameConsole.SetAudioSamplingRate( AUDIO_DAC_SAMPLING_RATE );
   
    m_Display.Init( PPU_HORIZONTAL_RESOLUTION, PPU_VERTICAL_PAL_RESOLUTION, 2*PPU_HORIZONTAL_RESOLUTION, 2*PPU_VERTICAL_PAL_RESOLUTION );

    m_Display.Clear();
    m_Display.Flip();
    m_Display.Clear();
    m_Display.Flip();

    m_AudioDac.SetQueryFrameCallBack( AudioDacQueryFrame, this );

    m_InputManager.SetJoystickCallBack( JoysticEvent, this );
    m_InputManager.SetConsoleControlCallBack( GameConsoleControl, this );
    m_InputManager.Init();
}

bool Emulator_t::Run()
{
    bool isExit = false;

    SDL_Event sdlEvent;

    while( SDL_PollEvent( &sdlEvent ) )
    {
        m_InputManager.Run( &sdlEvent );

        if( sdlEvent.type == SDL_WINDOWEVENT )
        {
            if( SDL_WINDOWEVENT_RESIZED == sdlEvent.window.event )
            {
                m_Display.WindowResize( sdlEvent.window.data1, sdlEvent.window.data2 );
            }

            if( SDL_WINDOWEVENT_CLOSE == sdlEvent.window.event )
            {
                isExit = true;
            }
        }
    }

    if( m_GameConsoleStarted )
    {
        uint64_t perfCounter = SDL_GetPerformanceCounter();
        uint64_t perfFreq = SDL_GetPerformanceFrequency();

        uint32_t msec = ( uint32_t )( 1000000 * perfCounter / perfFreq );
        
        m_GameConsole.Run( msec );

        ShowFps( m_GameConsole.GetFramesPerSecond() );
    }

    SDL_Delay( 0 );

    return isExit;
}

void Emulator_t::ShowFps( uint16_t fps )
{
    char fpsStr[20];
    sprintf( fpsStr, "FPS = %d", fps );
    m_Display.SetTitle( fpsStr );
}


void Emulator_t::AudioDacQueryFrame( _out_ void* pContext, _out_ int16_t* pData, _in_ uint16_t bytesCnt )
{
    Emulator_t* emulator = static_cast<Emulator_t *>(pContext);
    emulator->m_GameConsole.GetAudioFrame( pData, bytesCnt );
}

void Emulator_t::JoysticEvent( _in_ void* pContext, _in_ uint8_t joysticA, _in_ uint8_t joysticB )
{
    Emulator_t* emulator = static_cast<Emulator_t *>(pContext);
    emulator->m_GameConsole.SetButtonJoysticA( joysticA );
    emulator->m_GameConsole.SetButtonJoysticB( joysticB );
}

void Emulator_t::GameConsoleControl( _in_ void* pContext, _in_ ConsoleCommand_t command )
{
    Emulator_t* emulator = static_cast<Emulator_t *>(pContext);
    uint8_t* pGameContext;

    switch( command )
    {
        case GAME_CONSOLE_RESET_CMD:
             emulator->m_GameConsole.Init();
            // m_GameConsole.Reset();

        break;

        case GAME_CONSOLE_LOAD_GAME_ROM_CMD:
        {
            emulator->m_GameConsoleStarted = false;

           // m_RomManager.Unload();
            emulator->m_RomManager.SetDialogTitle( (char *)" Please Select ROM File " );
            emulator->m_RomManager.SetDialogFilter( (char *)"ROM images\0*.nes\0" );
            FileLoaderStatus_t fileStatus = emulator->m_RomManager.BrowseAndLoad(); // Выбор и открытие файла игры
           
            if( FILE_LOADED_SUCCESS_STATUS != fileStatus )
            {
                break;
            }
            
            emulator->m_GameConsole.Init();
          //  m_GameConsole.Reset();
            emulator->m_AudioDac.Init();

            emulator->m_GameConsoleStarted = true;
        }
        break;

        case GAME_CONSOLE_SAVE_GAME_CMD:
            
            pGameContext = new uint8_t[ GAME_CONTEXT_SIZE ];
            emulator->m_GameConsole.SaveGameContext( pGameContext );

        break;

        case GAME_CONSOLE_LOAD_GAME_CMD:
            
        break;

        default:
        
        break;
    }
}

void Emulator_t::RomFileAcces( _in_ void * pContext, _out_ uint8_t* pData, _in_ uint32_t offset, _in_ uint16_t bytesCnt )
{
    Emulator_t* emulator = static_cast<Emulator_t *>(pContext);
    uint8_t* pSource = emulator->m_RomManager.GetDataPointer() + offset;

    for ( uint16_t i = 0; i != bytesCnt; i++ ) // Подгружаем bytesCnt байт из файла игры в картридж
    {
        *pData++ = *pSource++;
    }
}

void Emulator_t::PresentFrame( _in_ void * pContext, uint8_t* pData, uint16_t len, uint16_t posInFrame )
{    
    Emulator_t* emulator = static_cast<Emulator_t *>(pContext);

    if( pData )
    {
        for ( uint16_t xVisible = 0; xVisible < PPU_HORIZONTAL_RESOLUTION; xVisible++ ) 
        {
            uint8_t color = pData[ xVisible ];
            color &= ~0xC0;
            //color = 255 - color;
            emulator->m_Display.DrawPixel( xVisible, posInFrame/PPU_HORIZONTAL_RESOLUTION, GetPalettePixel( color )->Red, GetPalettePixel( color )->Green, GetPalettePixel( color )->Blue );
        }
    }
    else
    {
        emulator->m_Display.Flip();
    }
}