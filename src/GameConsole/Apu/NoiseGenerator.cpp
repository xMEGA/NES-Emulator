/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "NoiseGenerator.h"

static const uint8_t LengthCounterTable[] =
{
    0x0A, 0xFE, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06, 0xA0, 0x08, 0x3C, 0x0A, 0x0E, 0x0C, 0x1A, 0x0E, 
    0x0C, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 0xC0, 0x18, 0x48, 0x1A, 0x10, 0x1C, 0x20, 0x1E
};

static const uint16_t NoisePeriodTable[] =
{
    0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0060, 0x0080, 0x00A0,
    0x00CA, 0x00FE, 0x017C, 0x01FC, 0x02FA, 0x03F8, 0x07F2, 0x0FE4
};     

void NoiseGenerator_t::Init( uint32_t nsPerQuant )
{
    m_nsPerQuant = nsPerQuant;
    m_LengthCounterUnitEnable = false;        
    m_LengthCounter = 0; 
    m_EnvelopeStart = false;
    m_ShiftRegister = 1;
    m_Timer = 0;    
}

void NoiseGenerator_t::WriteReg0( uint8_t value )
{
    m_ControlReg.Value  = value;
    m_EnvelopeReg.Value = value;

    if( m_EnvelopeReg.Constant )
    {
        m_Amplitude    = m_ControlReg.Amplitude;    
    }
    else
    {
        m_Amplitude = m_EnvAmplitude;
    }
}

void NoiseGenerator_t::WriteReg1( uint8_t value )
{
    m_NoiseReg1.Value = value;

    m_Period = NoisePeriodTable[ m_NoiseReg1.PeriodIdx ];
}

void NoiseGenerator_t::WriteReg2( uint8_t value )
{
    if( m_LengthCounterUnitEnable )
    {
        m_LengthCounter = LengthCounterTable[ value >> 3 ];
    }
    
    m_EnvelopeStart = true;
}

void NoiseGenerator_t::Enable( void )
{
    m_LengthCounterUnitEnable = true;
}

void NoiseGenerator_t::Disable( void )
{
    m_LengthCounterUnitEnable = false;
    //m_LengthCounter = 0;
}

uint16_t NoiseGenerator_t::GetQuant( void )
{
    uint32_t value;
    
    value = SequencerClock( TimerClock() );

    
    if( m_LengthCounterUnitEnable )
    {
        if( 0 == m_LengthCounter )
        {
            value = 0;
        }
    }


    return value * m_Amplitude;
}

void NoiseGenerator_t::EnvelopeUnitClock( void )
{
    if (m_EnvelopeStart) 
    {
        m_EnvelopeStart = false;
        m_EnvAmplitude = 15;
        m_EnvelopeDivider = m_EnvelopeReg.Period;
        if (!m_EnvelopeReg.Constant)
        {
            m_Amplitude = m_EnvAmplitude;
        }
    } 
    else 
    {

        if( m_EnvelopeDivider )
        {
            m_EnvelopeDivider--;
        }

        if ( 0 == m_EnvelopeDivider ) 
        {
            if (m_EnvAmplitude == 0) 
            {
                if ( m_EnvelopeReg.LoopEnable)
                {
                    m_EnvAmplitude = 15;
                }
            } 
            else
            {
                m_EnvAmplitude--;
            }


            if(!m_EnvelopeReg.Constant)
            {
                m_Amplitude = m_EnvAmplitude;
            }
            m_EnvelopeDivider = m_EnvelopeReg.Period;
        }
    }
}


void NoiseGenerator_t::LengthCounterUnitClock( void )
{
    if( 0 == m_ControlReg.LenCounterClkDisable ) 
    {
        if( m_LengthCounter )
        {
            m_LengthCounter --;
        }
    }
}



uint8_t NoiseGenerator_t::SequencerClock( uint8_t clk )
{
    uint16_t  feedBack = 0;

    while( clk )
    {
        if( m_NoiseReg1.Mode )
        {
            feedBack = ( m_ShiftRegister & 0x01 ) ^ ( (  m_ShiftRegister >> 6 ) & 0x01);
        }
        else
        {
            feedBack = ( m_ShiftRegister & 0x01 ) ^ ( (  m_ShiftRegister >> 1 ) & 0x01);
        }

        feedBack <<= 14;

        m_ShiftRegister >>= 1;

        m_ShiftRegister |= feedBack;
        clk --;
    }

    return !(m_ShiftRegister & 0x01);
}

uint32_t NoiseGenerator_t::TimerClock( void )
{
    uint16_t overflowCnt = 0;  // Количество переполнений счетчика
    m_Timer += m_nsPerQuant / 1024;

    while( m_Timer > m_Period )
    {
        if( m_Period == 0)
        {
            break;
        }

        m_Timer -= m_Period;
        overflowCnt ++;    
    }

    return overflowCnt;
}