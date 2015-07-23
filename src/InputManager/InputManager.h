/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _InputManager_h_
#define _InputManager_h_

#include <stdint.h>

enum ConsoleCommand_t
{
    EMULATOR_RESET_CMD         ,
    EMULATOR_LOAD_GAME_ROM_CMD ,
    EMULATOR_SAVE_GAME_CMD     ,
    EMULATOR_LOAD_GAME_CMD     ,
    EMULATOR_SAVE_VRAM_DUMP_CMD,
    EMULATOR_SAVE_CHR_DUMP_CMD ,
};

struct InputManagerInfo_t
{
    struct
    {
        uint8_t GamepadStateA;
        uint8_t GamepadStateB;
        bool    IsChanged;
    }GamePad;

    struct
    {
        uint32_t SizeX;
        uint32_t SizeY;
        bool     IsChanged;
    }Window;
    
    struct
    {
        ConsoleCommand_t Command;
        bool IsChanged;
    }User;
    
    struct
    {
        bool IsExit;
    }General;
};


class InputManager_t
{
public:
    void Init( void );
    InputManagerInfo_t Run();

private:
    uint8_t                     m_GamepadA;
    uint8_t                     m_GamepadB;
};
#endif