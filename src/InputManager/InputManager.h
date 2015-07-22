/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _InputManager_h_
#define _InputManager_h_

#include <stdint.h>
#include "SDL.h"


enum ConsoleCommand_t
{
    GAME_CONSOLE_RESET_CMD         ,
    GAME_CONSOLE_LOAD_GAME_ROM_CMD ,
    GAME_CONSOLE_SAVE_GAME_CMD     ,
    GAME_CONSOLE_LOAD_GAME_CMD     ,
    GAME_CONSOLE_SAVE_VRAM_DUMP_CMD,
    GAME_CONSOLE_SAVE_CHR_DUMP_CMD
};

typedef void (*GamepadCallBack)( void * pContext, uint8_t joystickA,  uint8_t joystickB );
typedef void (*ConsoleControlCallBack_t)( void * pContext, ConsoleCommand_t command );

class InputManager_t
{
public:
    void Init( void );
    void SetGamepadCallBack( GamepadCallBack joystickCallBack, void * pContext );
    void SetConsoleControlCallBack( ConsoleControlCallBack_t consoleControlCallBack, void * pContext );
    void Run( SDL_Event* SdlEvent );

private:
    uint8_t                     m_GamepadA;
    uint8_t                     m_GamepadB;
    void*                       m_pGamepadContext;
    GamepadCallBack             m_GamepadCallBack;
    void*                       m_pConsoleControlContext;
    ConsoleControlCallBack_t    m_ConsoleControlCallBack;
};
#endif