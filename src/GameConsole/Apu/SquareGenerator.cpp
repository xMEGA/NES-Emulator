/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "SquareGenerator.h"

#define SQUARE_SEQUENCER_NUMBER_OF_SEPS         8
#define SQUARE_SEQUENCER_NUMBER_OF_DUTY_CYCLE   5


static const uint8_t SequenceTable[ SQUARE_SEQUENCER_NUMBER_OF_DUTY_CYCLE ] = { 0x08, 0x0C, 0x0F, 0x30, 0x00 };
                                                                            // 12,5%   25%     50%  -75%    0%    

static const uint8_t LengthCounterTable[] =
{
    0x0A, 0xFE, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06, 0xA0, 0x08, 0x3C, 0x0A, 0x0E, 0x0C, 0x1A, 0x0E, 
    0x0C, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 0xC0, 0x18, 0x48, 0x1A, 0x10, 0x1C, 0x20, 0x1E
};

bool SquareGenerator_t::IsLengthCounter( void )
{
    if( m_LengthCounter )
        return true;
    else
        return false;
}

void SquareGenerator_t::Init( uint8_t nChannel, uint32_t nsPerQuant )
{
    m_ChannelNubmer = nChannel;
    m_nsPerQuant = nsPerQuant;
    m_TimerCounterValue = 0;
    m_SweepDivider      = 0;
    m_EnvelopeStart   = false;
    m_EnvAmplitude    = 0;
    m_EnvelopeDivider = 0;
    m_Amplitude          = 0;
    m_SequencerStep      = 0;
    m_LengthCounter      = 0;
    m_LengthCounterUnitEnable = false;
}

void SquareGenerator_t::WriteReg0( uint8_t value )
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

void SquareGenerator_t::WriteReg1( uint8_t value )
{
    m_SweepReg.Value      = value;
    m_SweepReload = true;
    CalculateSweep();
}


void SquareGenerator_t::WriteReg2( uint8_t value )
{
    m_PeriodReg.lowPart = value;
    CalculateSweep();
}

void SquareGenerator_t::WriteReg3( uint8_t value )
{
    m_PeriodReg.highPart = ( 0x07 & value );            

    if( m_LengthCounterUnitEnable )
    {
        m_LengthCounter = LengthCounterTable[ value >> 3 ];
    }

    m_EnvelopeStart = true;
    CalculateSweep();
}

void SquareGenerator_t::Enable( void )
{
    m_LengthCounterUnitEnable = true;
}

void SquareGenerator_t::Disable( void )
{
    m_LengthCounterUnitEnable = false;
    m_LengthCounter = 0;
}

uint16_t SquareGenerator_t::GetQuant( void )
{
    uint32_t value;
    uint8_t  amplitude;
    
    value = SequencerClock( TimerClock() );
    
    amplitude = m_Amplitude;

    if( m_LengthCounterUnitEnable )
    {
        if( 0 == m_LengthCounter )
        {
            value = 0;
        }
    }

    return amplitude * value;
}


void SquareGenerator_t::EnvelopeUnitClock( void )
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

uint32_t SquareGenerator_t::TimerClock( void )
{
    uint16_t overflowCnt = 0;
    m_TimerCounterValue += m_nsPerQuant / 1024;// ( ns/512 )/2;

    while( m_TimerCounterValue > m_PeriodReg.value )
    {
        
        if( m_PeriodReg.value == 0)
        {
            break;
        }

        m_TimerCounterValue -= m_PeriodReg.value;
        overflowCnt ++;
        
    }
    return overflowCnt;
}


void SquareGenerator_t::CalculateSweep( void )
{
    uint16_t shifter = 0;
        
    shifter = m_PeriodReg.value >> m_SweepReg.SweepStep;

    if( m_SweepReg.SweepNegative )
    {
        if( m_ChannelNubmer == 1 )
        {
          shifter = ~shifter;
        }
        else
        {
          shifter = -shifter;
        }
    }
    m_Period = m_PeriodReg.value + shifter;
    m_IsSweepPeriodCorrect = (m_PeriodReg.value > 7) && ( m_SweepReg.SweepNegative || ( m_Period < 0x0800 ) );
}

void SquareGenerator_t::SweepUnitClock( void )
{

    if( m_SweepReload )
    {
        m_SweepDivider = m_SweepReg.SweepPeriod;
        m_SweepReload = false;
    }

    if( m_SweepDivider )
    {
        m_SweepDivider--;
    }

    if( 0 == m_SweepDivider )
    {
        if( m_SweepReg.SweepEnable && m_SweepReg.SweepStep && m_IsSweepPeriodCorrect )
        {
             m_PeriodReg.value =  m_Period & 0x07FF;
        }
        m_SweepDivider = m_SweepReg.SweepPeriod;;
    }

    CalculateSweep();
}

void SquareGenerator_t::LengthCounterUnitClock( void )
{
    if( 0 == m_ControlReg.LenCounterClkDisable ) 
    {
        if( m_LengthCounter )
        {
            m_LengthCounter --;
        }
    }
}


uint8_t SquareGenerator_t::SequencerClock( uint8_t clk )
{
    uint8_t retValue;

    while( clk )
    {
        m_SequencerStep ++;

        if( m_SequencerStep >= SQUARE_SEQUENCER_NUMBER_OF_SEPS)
        {
            m_SequencerStep = 0;
        }

        clk --;
    }

    retValue = 0x01 & ( SequenceTable[ m_ControlReg.DutyCycle ] >> m_SequencerStep );

    return retValue;
}
