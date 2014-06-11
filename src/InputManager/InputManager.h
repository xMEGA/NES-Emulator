/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _InputManager_h_
#define _InputManager_h_

#include <stdint.h>
#include "SDL.h"

#define _in_
#define _out_
#define _in_out_


enum ConsoleCommand_t
{
    GAME_CONSOLE_RESET_CMD,
    GAME_CONSOLE_LOAD_GAME_ROM_CMD,
    GAME_CONSOLE_SAVE_GAME_CMD,
    GAME_CONSOLE_LOAD_GAME_CMD,

};

typedef void (*JoystickCallBack_t)( _in_ void * pContext, _in_ uint8_t joystickA,  _in_ uint8_t joystickB );
typedef void (*ConsoleControlCallBack_t)( _in_ void * pContext, _in_ ConsoleCommand_t command );

class InputManager_t
{
public:
    void Init( void );
    void SetJoystickCallBack( JoystickCallBack_t joystickCallBack, _in_ void * pContext );
    void SetConsoleControlCallBack( ConsoleControlCallBack_t consoleControlCallBack, _in_ void * pContext );
    void Run( SDL_Event* SdlEvent );

private:
    uint8_t                     m_JoystickA;
    uint8_t                     m_JoystickB;
    void*                       m_pJoysticContext;
    JoystickCallBack_t          m_JoystickCallBack;
    void*                       m_pConsoleControlContext;
    ConsoleControlCallBack_t    m_ConsoleControlCallBack;
};
#endif