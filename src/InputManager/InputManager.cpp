/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "InputManager.h"
#include "../GameConsole/Control/Control.h"
#include "SDL.h"

void InputManager_t::Init(void)
{
    m_GamepadA = 0;
    m_GamepadB = 0;
}

InputManagerInfo_t InputManager_t::Run()
{
    
    InputManagerInfo_t inputManagerInfo;

    inputManagerInfo.GamePad.IsChanged = false;
    inputManagerInfo.Window.IsChanged = false;
    inputManagerInfo.General.IsExit = false;
    inputManagerInfo.User.IsChanged = false;
    
    SDL_Event sdlEvent;

    while( SDL_PollEvent( &sdlEvent ) )
    {
        

        if( sdlEvent.type == SDL_WINDOWEVENT )
        {
            if( SDL_WINDOWEVENT_RESIZED == sdlEvent.window.event )
            {
                inputManagerInfo.Window.SizeX = sdlEvent.window.data1;
                inputManagerInfo.Window.SizeY = sdlEvent.window.data2;
                inputManagerInfo.Window.IsChanged = true;
            }

            if( SDL_WINDOWEVENT_CLOSE == sdlEvent.window.event )
            {
                inputManagerInfo.General.IsExit = true;
            }
        }
        
        if (sdlEvent.type == SDL_KEYDOWN)
        {
            switch( sdlEvent.key.keysym.sym )
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
                    inputManagerInfo.User.IsChanged = true;
                    inputManagerInfo.User.Command = EMULATOR_RESET_CMD;
                break;

                case SDLK_F5:
                    inputManagerInfo.User.IsChanged = true;
                    inputManagerInfo.User.Command = EMULATOR_LOAD_GAME_ROM_CMD;
                break;

                case SDLK_F8:
                    inputManagerInfo.User.IsChanged = true;
                    inputManagerInfo.User.Command = EMULATOR_SAVE_VRAM_DUMP_CMD;
                break;  

                case SDLK_F7:
                    inputManagerInfo.User.IsChanged = true;
                    inputManagerInfo.User.Command = EMULATOR_SAVE_CHR_DUMP_CMD;
                break;  

                case SDLK_F10:
                    inputManagerInfo.User.IsChanged = true;
                    inputManagerInfo.User.Command = EMULATOR_SAVE_GAME_CMD;
                 break;  

                case SDLK_F11:
                    inputManagerInfo.User.IsChanged = true;
                    inputManagerInfo.User.Command = EMULATOR_LOAD_GAME_CMD;
                break;  

                default:
                break;

            }

            inputManagerInfo.GamePad.GamepadStateA = m_GamepadA;
            inputManagerInfo.GamePad.GamepadStateB = m_GamepadB;
            inputManagerInfo.GamePad.IsChanged = true;
        }

        if( sdlEvent.type == SDL_KEYUP )
        {
            switch( sdlEvent.key.keysym.sym )
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

            inputManagerInfo.GamePad.GamepadStateA = m_GamepadA;
            inputManagerInfo.GamePad.GamepadStateB = m_GamepadB;
            inputManagerInfo.GamePad.IsChanged = true;
        }
        
    }
    
    return inputManagerInfo;
}