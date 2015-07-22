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
   
    m_Display.Init( PPU_HORIZONTAL_RESOLUTION, PPU_VERTICAL_PAL_RESOLUTION, 2 * PPU_HORIZONTAL_RESOLUTION, 2 * PPU_VERTICAL_PAL_RESOLUTION, false );

    m_Display.Clear();
    m_Display.Flip();
    m_Display.Clear();
    m_Display.Flip();

    m_AudioDac.SetQueryFrameCallBack( AudioDacQueryFrame, this );
    m_AudioDac.Init();

    m_InputManager.SetGamepadCallBack( GamepadEvent, this );
    m_InputManager.SetConsoleControlCallBack( GameConsoleControlCallBack, this );
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
    char fpsStr[ 20 ];
    sprintf( fpsStr, "FPS = %d", fps );
    m_Display.SetTitle( fpsStr );
}


void Emulator_t::AudioDacQueryFrame( void* pContext, int16_t* pData, uint16_t bytesCnt )
{
    Emulator_t* pEmulator = static_cast<Emulator_t *>(pContext);
    pEmulator->m_GameConsole.GetAudioFrame( pData, bytesCnt );
}

void Emulator_t::GamepadEvent( void* pContext, uint8_t gamepadA, uint8_t gamepadB )
{
    Emulator_t* pEmulator = static_cast<Emulator_t *>(pContext);
    pEmulator->m_GameConsole.SetButtonGamepadA( gamepadA );
    pEmulator->m_GameConsole.SetButtonGamepadB( gamepadB );
}


void Emulator_t::GameConsoleControlCallBack( void* pContext, ConsoleCommand_t command )
{
    Emulator_t* pEmulator = static_cast< Emulator_t* >( pContext );
    pEmulator->GameConsoleControl( command );
}

void Emulator_t::GameConsoleControl( ConsoleCommand_t command )
{
    
    uint8_t* pGameContext;

    switch( command )
    {
        case GAME_CONSOLE_RESET_CMD:
             m_GameConsole.Init();
            // m_GameConsole.Reset();

        break;

        case GAME_CONSOLE_LOAD_GAME_ROM_CMD:
        {
            m_GameConsoleStarted = false;

           // m_RomManager.Unload();
            m_RomManager.SetDialogTitle( (char *)" Please Select ROM File " );
            m_RomManager.SetDialogFilter( (char *)"ROM images\0*.nes\0" );
            FileLoaderStatus_t fileStatus = m_RomManager.BrowseAndLoad(); // Выбор и открытие файла игры
           
            if( FILE_LOADED_SUCCESS_STATUS != fileStatus )
            {
                break;
            }
            
            m_GameConsole.Init();
          //  m_GameConsole.Reset();
            m_AudioDac.Init();

            m_GameConsoleStarted = true;
        }
        break;

        case GAME_CONSOLE_SAVE_GAME_CMD:
            
            pGameContext = new uint8_t[ GAME_CONTEXT_SIZE ];
            m_GameConsole.SaveGameContext( pGameContext );

        break;

        case GAME_CONSOLE_LOAD_GAME_CMD:
            
        break;

        case GAME_CONSOLE_SAVE_VRAM_DUMP_CMD:
        {
            FILE* pFile = fopen( "VideoRam.dmp", "wb" );
            
            if( NULL != pFile )
            {
                uint8_t buffer[ CARTRIDGE_PPU_VRAM_SIZE ];
                m_GameConsole.DumpPpuMemory( buffer, CARTRIDGE_PPU_VRAM_BASE_ADDR, sizeof( buffer ) );
                fwrite( buffer, sizeof( uint8_t ), sizeof( buffer ), pFile );
                fclose( pFile );
            }
        }
        break;
        
            
        case GAME_CONSOLE_SAVE_CHR_DUMP_CMD:
        {
            FILE* pFile = fopen( "PpuChr.dmp", "wb" );
            
            if( NULL != pFile )
            {
                uint8_t buffer[ CARTRIDGE_VIDEO_ROM_SIZE ];
                m_GameConsole.DumpPpuMemory( buffer, CARTRIDGE_VIDEO_ROM_BASE_PPU_ADDR, sizeof( buffer ) );
                fwrite( buffer, sizeof( uint8_t ), sizeof( buffer ), pFile );
                fclose( pFile );
            }
        }
        break;
        
        default:
        
        break;
    }
}

void Emulator_t::RomFileAcces( void * pContext, uint8_t* pData, uint32_t offset, uint16_t bytesCnt )
{
    Emulator_t* emulator = static_cast<Emulator_t *>(pContext);
    uint8_t* pSource = emulator->m_RomManager.GetDataPointer() + offset;

    for ( uint16_t i = 0; i != bytesCnt; i++ ) // Подгружаем bytesCnt байт из файла игры в картридж
    {
        *pData++ = *pSource++;
    }
}

void Emulator_t::PresentFrame( void * pContext, uint8_t* pData, uint16_t len, uint16_t posInFrame )
{    
    Emulator_t* pEmulator = static_cast<Emulator_t *>(pContext);


#ifdef DISPLAY_332

    static uint8_t* pPixel = ( uint8_t* )System->GetDisplay()->GetFrameBuffer();
    static uint8_t* pPalette = GetPalettePixel332();
    static uint32_t displayAlign = System->GetDisplay()->GetSizeHorizontal() - PPU_HORIZONTAL_RESOLUTION;

    if( 0 == posInFrame )
    {
        System->GetDisplay()->Flip();
        pPixel = ( uint8_t* )System->GetDisplay()->GetFrameBuffer();
    }
    	
    for( uint16_t xVisible = 0; xVisible < PPU_HORIZONTAL_RESOLUTION; xVisible++ ) 
    {
        *pPixel ++= pPalette[ *pData++ ];
    }

    pPixel += displayAlign;

#else

    static uint32_t* pPixel = ( uint32_t* )pEmulator->m_Display.GetFrameBuffer();
    static uint32_t* pPalette = GetPalettePixelRGBA();
    static uint32_t  displayAlign = pEmulator->m_Display.GetSizeHorizontal() - PPU_HORIZONTAL_RESOLUTION;

    if( 0 == posInFrame )
    {
        pEmulator->m_Display.Flip();
        pPixel = ( uint32_t* )pEmulator->m_Display.GetFrameBuffer();
    }
    
    for( uint16_t xVisible = 0; xVisible < PPU_HORIZONTAL_RESOLUTION; xVisible++ ) 
    {
        *pPixel ++= pPalette[ *pData++ ];
    }

    pPixel += displayAlign;

#endif

}