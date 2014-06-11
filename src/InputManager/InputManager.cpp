/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "InputManager.h"
#include "../GameConsole/Control/Control.h"

void InputManager_t::Init(void)
{
    m_JoystickA = 0;
    m_JoystickB = 0;
}

void InputManager_t::SetJoystickCallBack( JoystickCallBack_t joystickCallBack, _in_ void * pContext )
{
    m_pJoysticContext  = pContext;
    m_JoystickCallBack = joystickCallBack;
}

void InputManager_t::SetConsoleControlCallBack( ConsoleControlCallBack_t consoleControlCallBack, _in_ void * pContext )
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
                m_JoystickA |= JOYSTIC_BUTTON_UP;
            break;
                
            case SDLK_s:
                m_JoystickA |= JOYSTIC_BUTTON_DOWN;
            break;
    
            case SDLK_a:
                m_JoystickA |= JOYSTIC_BUTTON_LEFT;
            break;
    
            case SDLK_d:
                m_JoystickA |= JOYSTIC_BUTTON_RIGHT;
            break;
    
            case SDLK_m:
                m_JoystickA |= JOYSTIC_BUTTON_A;
            break;

            case SDLK_n:
                m_JoystickA |= JOYSTIC_BUTTON_B;
            break;

            case SDLK_RETURN:
                m_JoystickA |= JOYSTIC_BUTTON_START;
            break;
                    
            case SDLK_RSHIFT:
                m_JoystickA |= JOYSTIC_BUTTON_SELECT;
            break;

            case SDLK_F1:
                m_ConsoleControlCallBack( m_pConsoleControlContext, GAME_CONSOLE_RESET_CMD );
            break;

            case SDLK_F5:
                m_ConsoleControlCallBack( m_pConsoleControlContext, GAME_CONSOLE_LOAD_GAME_ROM_CMD );
            break;


            default:break;

        }

        m_JoystickCallBack( m_pJoysticContext, m_JoystickA, m_JoystickB);
    
    }

    if (SdlEvent->type == SDL_KEYUP)
    {
        switch( SdlEvent->key.keysym.sym )
        {
            case SDLK_w:
                m_JoystickA &= ~JOYSTIC_BUTTON_UP;
            break;
                
            case SDLK_s:
                m_JoystickA &= ~JOYSTIC_BUTTON_DOWN;
            break;
    
            case SDLK_a:
                m_JoystickA &= ~JOYSTIC_BUTTON_LEFT;
            break;
    
            case SDLK_d:
                m_JoystickA &= ~JOYSTIC_BUTTON_RIGHT;
            break;
    
            case SDLK_m:
                m_JoystickA &= ~JOYSTIC_BUTTON_A;
            break;

            case SDLK_n:
                m_JoystickA &= ~JOYSTIC_BUTTON_B;
            break;

            case SDLK_RETURN:
                m_JoystickA &= ~JOYSTIC_BUTTON_START;
            break;
                                        
            case SDLK_RSHIFT:
                m_JoystickA &= ~JOYSTIC_BUTTON_SELECT;
            break;

            default:break;

        }

         m_JoystickCallBack( m_pJoysticContext, m_JoystickA, m_JoystickB);
    
    }
}