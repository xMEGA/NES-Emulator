/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Emulator.h"
#include <stdio.h>
#include "Utils/TimeCounter.h"
#include "FileManager/FileOpenDialog.h"

void Emulator_t::Init()
{
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

    m_InputManager.Init();
}

bool Emulator_t::Run()
{
    InputManagerInfo_t inputManagerInfo = m_InputManager.Run();
         
    if( true == inputManagerInfo.User.IsChanged )
    {
        UserControl( inputManagerInfo.User.Command );
    }
    
    if( m_GameConsoleStarted )
    {
          
        if( true == inputManagerInfo.Window.IsChanged )
        {
            m_Display.WindowResize( inputManagerInfo.Window.SizeX, inputManagerInfo.Window.SizeY );
        }
                
        if( true == inputManagerInfo.GamePad.IsChanged )
        {
            m_GameConsole.SetButtonGamepadA( inputManagerInfo.GamePad.GamepadStateA );
            m_GameConsole.SetButtonGamepadB( inputManagerInfo.GamePad.GamepadStateB );
        }
        
		uint32_t msec = TimeCounterGetMsec();
        m_GameConsole.Run( msec );

        ShowFps( m_GameConsole.GetFramesPerSecond() );
    }

    return inputManagerInfo.General.IsExit;
}

void Emulator_t::ShowFps( uint16_t fps )
{
    char fpsStr[ 20 ];
    sprintf( fpsStr, "FPS = %d", fps );
    m_Display.SetTitle( fpsStr );
}

void Emulator_t::UserControl( ConsoleCommand_t command )
{

    switch( command )
    {
        case EMULATOR_RESET_CMD:
             m_GameConsole.Init();
        break;

        case EMULATOR_LOAD_GAME_ROM_CMD:
        {
            m_GameConsoleStarted = false;

           // m_RomManager.Unload();
			
            FileOpenDialog_t fileOpenDialog;

            std::string filePath = fileOpenDialog.Browse( " Please Select ROM File ", "ROM images\0*.nes\0" );

            FileStatus_t fileStatus = m_RomManager.Load( filePath );
           
            if( FILE_SUCCESS_STATUS != fileStatus )
            {
                break;
            }
            
            m_GameConsole.Init();
            m_AudioDac.Init();
            m_GameConsoleStarted = true;
        }
        break;

        case EMULATOR_SAVE_GAME_CMD:
        {
            
//            m_RomManager.ResetSaveFile();
            
            uint8_t buffer[ 1024 ];
               
            uint32_t len = 0;
            
            do
            {
                
                uint32_t len = m_GameConsole.SaveGameContext( buffer, sizeof( buffer ) );
                
                if( len > 0 )
                {
                    //m_RomManager.AppendToSaveFile( buffer, len );
                }
                
            }while( len > 0 );
        }
        break;

        case EMULATOR_LOAD_GAME_CMD:
            
        break;

        case EMULATOR_SAVE_VRAM_DUMP_CMD:
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
        
            
        case EMULATOR_SAVE_CHR_DUMP_CMD:
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

void Emulator_t::AudioDacQueryFrame( void* pContext, int16_t* pData, uint16_t bytesCnt )
{
    Emulator_t* pEmulator = static_cast< Emulator_t* >( pContext );
    pEmulator->m_GameConsole.GetAudioFrame( pData, bytesCnt );
}

void Emulator_t::RomFileAcces( void * pContext, uint8_t* pData, uint32_t offset, uint16_t bytesCnt )
{
    Emulator_t* emulator = static_cast< Emulator_t* >( pContext );
    uint8_t* pSource = emulator->m_RomManager.GetDataPointer() + offset;

    for ( uint16_t i = 0; i != bytesCnt; i++ ) // Подгружаем bytesCnt байт из файла игры в картридж
    {
        *pData++ = *pSource++;
    }
}

void Emulator_t::PresentFrame( void * pContext, uint8_t* pData, uint16_t len, uint16_t posInFrame )
{    
    Emulator_t* pEmulator = static_cast< Emulator_t* >( pContext );


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