/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Control.h"


void Control_t::Reset(void)
{
    m_LatchACounter = 0;
    m_LatchBCounter = 0;
}

void Control_t::SetButtonGamepadA( uint8_t button )
{
    m_PortAdata = button;
}

void Control_t::SetButtonGamepadB( uint8_t button )
{
    m_PortBdata = button;
}

uint8_t Control_t::Read( uint16_t addr )
{
    uint8_t retValue = 0;    
    switch( addr )
    {
        case CONTROL_PORTA_ADDR:    
            retValue = CONTROL_GAMEPAD_DATA_BIT & (m_PortAlatchedData >> m_LatchACounter);
            m_LatchACounter ++;
        break;

        case CONTROL_PORTB_ADDR:

            retValue = 0;
            

        break;

        default:
        
        break;
    }

    return retValue;
}

void Control_t::Write( uint16_t addr, uint8_t data)
{
    
    
    switch( addr )
    { 
        case CONTROL_PORTA_ADDR:
            if( data & CONTROL_LATCH_LINE_BIT) // Latch
            {
                m_PortAlatchedData = m_PortAdata;
                m_PortBlatchedData = m_PortBdata;

                m_LatchACounter       = 0;
                m_LatchBCounter    = 0;
            }
        break;

        case CONTROL_PORTB_ADDR:
        //    bp = 0;
        break;

        default:

        break;
    }

}