/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Emulator.h"
#include <stdio.h>
#include "Utils/TimeCounter.h"
#include "FileManager/FileOpenDialog.h"
#include "Utils/File.h"

void Emulator_t::Init()
{
    m_GameConsoleStarted = false;

    m_GameConsole.SetPresentScanLineCallBack( PresentScanLineCallBack, this );
//    m_GameConsole.SetRomFileAccesCallBack( RomFileAcces, this );
    m_GameConsole.SetAudioSamplingRate( AUDIO_DAC_SAMPLING_RATE );
   
    m_Display.Init( PPU_HORIZONTAL_RESOLUTION, PPU_VERTICAL_RESOLUTION, 2 * PPU_HORIZONTAL_RESOLUTION, 2 * PPU_VERTICAL_RESOLUTION, false );

    m_Display.Clear();
    m_Display.Flip();
    m_Display.Clear();
    m_Display.Flip();

    m_AudioDac.SetQueryFrameCallBack( AudioDacQueryFrame, this );
    m_AudioDac.Init();

    m_InputManager.Init();
}

uint32_t FrameCounter = 0;
uint32_t Fps = 0;

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
        
        
        m_GameConsole.ProcessingOneFrame( msec );

        Fps = m_GameConsole.GetFramesPerSecond();
       
        FrameCounter++;
        
        if( FrameCounter > 20000 )
        {
            printf( "FPS=%d\n", Fps );
            fflush( stdout );
            inputManagerInfo.General.IsExit = true;
        }
        
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

void Emulator_t::SaveGameContext()
{
    std::string romFileName = m_RomManager.GetFilePath();
            
    romFileName.replace( romFileName.find( "nes" ), 3, "sev" );

    File_t saveFile;

    bool isOpenedSuccess = saveFile.OpenForWrite( romFileName );

    if( true == isOpenedSuccess )
    {
        uint32_t len = 0;

        uint8_t* pSaveBuffer = new uint8_t [ SAVE_FILE_MAX_SIZE ];

        if( NULL != pSaveBuffer )
        {
            len = m_GameConsole.SaveGameContext( pSaveBuffer, SAVE_FILE_MAX_SIZE );
                
            if( len > 0 )
            {
                saveFile.Write( pSaveBuffer, len );
            }

            delete[] pSaveBuffer;
        }

        saveFile.Close();
    }
}

void Emulator_t::LoadGameContext()
{
    std::string romFileName = m_RomManager.GetFilePath();
            
    romFileName.replace( romFileName.find( "nes" ), 3, "sev" );
   
    File_t saveFile;

    bool isOpenedSuccess = saveFile.OpenForRead( romFileName );

    if( true == isOpenedSuccess )
    {
        uint32_t size = saveFile.GetSize();

        uint8_t* pSaveFileBuffer = new uint8_t[ size ];
        
        if( NULL != pSaveFileBuffer )
        {
            size = saveFile.Read( pSaveFileBuffer, size );

            m_GameConsole.LoadGameContext( pSaveFileBuffer, size );

            delete[] pSaveFileBuffer;
        }

        saveFile.Close();
    }
}

void Emulator_t::LoadGameRom()
{
    m_GameConsoleStarted = false;
			
    FileOpenDialog_t fileOpenDialog;

    std::string filePath = fileOpenDialog.Browse( " Please Select ROM File ", "ROM images\0*.nes\0" );

    bool isLoadedSuccess = m_RomManager.Load( filePath );
           
    if( false == isLoadedSuccess )
    {
//        break;
    }

    m_GameConsole.Init();
    m_GameConsole.LoadGameRomFile( m_RomManager.GetDataPointer(), m_RomManager.GetRomSize() );
    m_AudioDac.Init();
    m_GameConsoleStarted = true;
    
    FrameCounter = 0;
    Fps = 0;
}
    
void Emulator_t::SaveVramDump()
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

void Emulator_t::SaveChrDump()
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


void Emulator_t::UserControl( ConsoleCommand_t command )
{

    switch( command )
    {
        case EMULATOR_RESET_CMD:
             m_GameConsole.Init();
        break;

        case EMULATOR_LOAD_GAME_ROM_CMD:
            LoadGameRom();    
        break;

        case EMULATOR_SAVE_GAME_CONTEXT_CMD:
            SaveGameContext();
        break;

        case EMULATOR_LOAD_GAME_CONTEXT_CMD:
            LoadGameContext();
        break;

        case EMULATOR_SAVE_VRAM_DUMP_CMD:
            SaveVramDump();
        break;
        
            
        case EMULATOR_SAVE_CHR_DUMP_CMD:
            SaveChrDump();
        break;
        
        default:
        
        break;
    }
}

void Emulator_t::AudioDacQueryFrame(void* pContext, int16_t* pData, uint16_t bytesCnt)
{
	Emulator_t* pEmulator = static_cast<Emulator_t*>(pContext);

	if( true == pEmulator->m_GameConsoleStarted )
	{
		pEmulator->m_GameConsole.GetAudioFrame(pData, bytesCnt);
	}
}

void Emulator_t::RomFileAcces( void* pContext, uint8_t* pData, uint32_t offset, uint16_t bytesCnt )
{
    Emulator_t* pEmulator = static_cast< Emulator_t* >( pContext );
    uint8_t* pSource = pEmulator->m_RomManager.GetDataPointer() + offset;

    for ( uint16_t i = 0; i != bytesCnt; i++ ) // Подгружаем bytesCnt байт из файла игры в картридж
    {
        *pData++ = *pSource++;
    }
}

void Emulator_t::PresentScanLineCallBack( void* pContext, uint8_t* pData, uint16_t scanLine )
{    
    //UNUSED( len );

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

    for( uint16_t xVisible = 0; xVisible < PPU_HORIZONTAL_RESOLUTION; xVisible++ ) 
    {
        *pPixel ++= pPalette[ *pData++ ];
    }

    pPixel += displayAlign;

     if( 0 == scanLine )
    {
        pEmulator->m_Display.Flip();
        pPixel = ( uint32_t* )pEmulator->m_Display.GetFrameBuffer();
    }

#endif

}
