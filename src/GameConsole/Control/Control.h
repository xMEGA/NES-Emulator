/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _Control_h_
#define _Control_h_

#include <stdint.h>

#define CONTROL_LATCH_LINE_BIT      0x01
#define CONTROL_GAMEPAD_DATA_BIT    0x01


#define CONTROL_PORTA_ADDR          0x4016
#define CONTROL_PORTB_ADDR          0x4017

enum GamepadButton_t
{
    GAMEPAD_BUTTON_A      = 0x01,
    GAMEPAD_BUTTON_B      = 0x02,
    GAMEPAD_BUTTON_SELECT = 0x04,
    GAMEPAD_BUTTON_START  = 0x08,
    GAMEPAD_BUTTON_UP     = 0x10,
    GAMEPAD_BUTTON_DOWN   = 0x20,
    GAMEPAD_BUTTON_LEFT   = 0x40,
    GAMEPAD_BUTTON_RIGHT  = 0x80
};


class Control_t
{

public:
    void SetButtonGamepadA( uint8_t button );
    void SetButtonGamepadB( uint8_t button );

public:
    void Reset(void);
    uint8_t Read( uint16_t addr );
    void Write( uint16_t addr, uint8_t data);

private:
    uint8_t  m_LatchACounter;
    uint8_t  m_LatchBCounter;
    
    uint8_t  m_PortAdata;         // Current PORTA state
    uint8_t  m_PortBdata;         // Current PORTB state

    uint32_t m_PortAlatchedData; //Buttons state after latch PORTA
    uint32_t m_PortBlatchedData; //Buttons state after latch PORTA
};


#endif