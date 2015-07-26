/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include  "GameConsole/GameConsole.h"
#include  "FileManager/RomFileManager.h"
#include  "Display/Display.h"
#include  "InputManager/InputManager.h"
#include  "AudioDac/AudioDac.h"

#define SAVE_FILE_MAX_SIZE 1024 * 1024

class Emulator_t
{
public:
    void Init();
    bool Run();

private:
    void ShowFps( uint16_t fps );
    void UserControl( ConsoleCommand_t command );
    void SaveGameContext();
    void LoadGameContext();
    void LoadGameRom();
    void SaveVramDump();
    void SaveChrDump();


private:
    static void RomFileAcces( void* pContext, uint8_t* pData, uint32_t offset, uint16_t bytesCnt );
    static void PresentFrame( void* pContext, uint8_t* pData, uint16_t len, uint16_t posInFrame );
    static void AudioDacQueryFrame( void* pContext, int16_t* pData, uint16_t bytesCnt );
  
private:
    RomFileManager_t  m_RomManager;
    GameConsole_t     m_GameConsole;
    Display_t         m_Display;
    AudioDac_t        m_AudioDac;
    InputManager_t    m_InputManager;
    bool	          m_GameConsoleStarted;
};
#endif