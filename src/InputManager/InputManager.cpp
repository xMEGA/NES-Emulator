/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "InputManager.h"
#include "../GameConsole/Control/Control.h"

void InputManager_t::Init(void)
{
    m_GamepadA = 0;
    m_GamepadB = 0;
}

void InputManager_t::SetGamepadCallBack( GamepadCallBack GamepadCallBack, void* pContext )
{
    m_pGamepadContext  = pContext;
    m_GamepadCallBack = GamepadCallBack;
}

void InputManager_t::SetConsoleControlCallBack( ConsoleControlCallBack_t consoleControlCallBack, void* pContext )
{
    m_pConsoleControlContext = pContext;
    m_ConsoleControlCallBack = consoleControlCallBack;
}


void InputManager_t::Run(SDL_Event* SdlEvent)
{
    if (SdlEvent->type == SDL_KEYDOWN)
    {
        switch( SdlEvent->key.keysym.sym )
        {
            case SDLK_w:
                m_GamepadA |= GAMEPAD_BUTTON_UP;
            break;
                
            case SDLK_s:
                m_GamepadA |= GAMEPAD_BUTTON_DOWN;
            break;
    
            case SDLK_a:
                m_GamepadA |= GAMEPAD_BUTTON_LEFT;
            break;
    
            case SDLK_d:
                m_GamepadA |= GAMEPAD_BUTTON_RIGHT;
            break;
    
            case SDLK_m:
                m_GamepadA |= GAMEPAD_BUTTON_A;
            break;

            case SDLK_n:
                m_GamepadA |= GAMEPAD_BUTTON_B;
            break;

            case SDLK_RETURN:
                m_GamepadA |= GAMEPAD_BUTTON_START;
            break;
                    
            case SDLK_RSHIFT:
                m_GamepadA |= GAMEPAD_BUTTON_SELECT;
            break;

            case SDLK_F1:
                m_ConsoleControlCallBack( m_pConsoleControlContext, GAME_CONSOLE_RESET_CMD );
            break;

            case SDLK_F5:
                m_ConsoleControlCallBack( m_pConsoleControlContext, GAME_CONSOLE_LOAD_GAME_ROM_CMD );
            break;

            case SDLK_F8:
                m_ConsoleControlCallBack( m_pConsoleControlContext, GAME_CONSOLE_SAVE_VRAM_DUMP_CMD );        
            break;  
            
            case SDLK_F7:
                m_ConsoleControlCallBack( m_pConsoleControlContext, GAME_CONSOLE_SAVE_CHR_DUMP_CMD );        
            break;  
            
            default:
            break;

        }

        m_GamepadCallBack( m_pGamepadContext, m_GamepadA, m_GamepadB );
    
    }

    if (SdlEvent->type == SDL_KEYUP)
    {
        switch( SdlEvent->key.keysym.sym )
        {
            case SDLK_w:
                m_GamepadA &= ~GAMEPAD_BUTTON_UP;
            break;
                
            case SDLK_s:
                m_GamepadA &= ~GAMEPAD_BUTTON_DOWN;
            break;
    
            case SDLK_a:
                m_GamepadA &= ~GAMEPAD_BUTTON_LEFT;
            break;
    
            case SDLK_d:
                m_GamepadA &= ~GAMEPAD_BUTTON_RIGHT;
            break;
    
            case SDLK_m:
                m_GamepadA &= ~GAMEPAD_BUTTON_A;
            break;

            case SDLK_n:
                m_GamepadA &= ~GAMEPAD_BUTTON_B;
            break;

            case SDLK_RETURN:
                m_GamepadA &= ~GAMEPAD_BUTTON_START;
            break;
                                        
            case SDLK_RSHIFT:
                m_GamepadA &= ~GAMEPAD_BUTTON_SELECT;
            break;

            default:break;

        }

         m_GamepadCallBack( m_pGamepadContext, m_GamepadA, m_GamepadB);
    
    }
}