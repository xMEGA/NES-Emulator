/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "TriangleGenerator.h"


#define TRIANGLE_SEQUENCER_NUMBER_OF_SEPS            32


static const uint8_t LengthCounterTable[] =
{
    0x0A, 0xFE, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06, 0xA0, 0x08, 0x3C, 0x0A, 0x0E, 0x0C, 0x1A, 0x0E, 
    0x0C, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 0xC0, 0x18, 0x48, 0x1A, 0x10, 0x1C, 0x20, 0x1E
};


static const uint8_t SequenceTable[] = 
{
    15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
};

void TriangleGenerator_t::WriteReg0( uint8_t value )
{
    m_TriangleReg0.Value = value;
}

void TriangleGenerator_t::WriteReg1( uint8_t value )
{
    m_PeriodReg.lowPart = value;
}

void TriangleGenerator_t::WriteReg2( uint8_t value )
{
    m_PeriodReg.highPart = ( 0x07 & value );

    if( m_LengthCounterUnitEnable )
    {
        m_LengthCounter = LengthCounterTable[ value >> 3 ];
    }
    m_LinerCounterHalt = true;
}


void TriangleGenerator_t::Init( uint32_t nsPerQuant )
{
    m_nsPerQuant = nsPerQuant;
    m_LengthCounterUnitEnable = false;        
    m_LengthCounter = 0; 

    m_LinerCounterHalt = false;
    m_LinearCounter = 0;
    
 
    m_PeriodReg.value = 0;
    m_TriangleReg0.Control = false;
    m_TriangleReg0.LinearCounterLoad = 0;
    m_TimerCounterValue = 0;    

 
    m_SequencerStep = 0;    //Sequencer state  ( 0..32 )
}

void TriangleGenerator_t::Enable( void )
{
    m_LengthCounterUnitEnable = true;
}

void TriangleGenerator_t::Disable( void )
{
    m_LengthCounterUnitEnable = false;
    //m_LengthCounter = 0;
}

uint16_t TriangleGenerator_t::GetQuant( void )
{
    uint32_t value;

    value = SequencerClock( TimerClock() );
    return value;
}


void TriangleGenerator_t::LinearCounterClock( void )
{
     if( m_LinerCounterHalt ) 
     {
         m_LinearCounter = m_TriangleReg0.LinearCounterLoad;
         m_LinerCounterHalt = false;//!!!
     } 
     else if( m_LinearCounter ) 
     {
        m_LinearCounter--;
     }
     if (!m_TriangleReg0.Control )
     {
        m_LinerCounterHalt = false;
     }
}


void TriangleGenerator_t::LengthCounterUnitClock( void )
{
     if ((!m_TriangleReg0.Control) && ( m_LengthCounter ) ) 
     {
        m_LengthCounter--;
     }
}



uint8_t TriangleGenerator_t::SequencerClock( uint8_t clk )
{

    while( clk )
    {
        if ( m_PeriodReg.value > 0 ) 
        {
            
            if ((m_LinearCounter > 0) && (m_LengthCounter > 0)) 
            {
                m_Amplitude = SequenceTable[ m_SequencerStep ];
                m_SequencerStep ++;
                
                if( m_SequencerStep >=31)
                    m_SequencerStep = 0;
            }
            
        } 
        else 
        {
            if ((m_LinearCounter > 0) && (m_LengthCounter > 0))
            {
                    m_SequencerStep = (m_SequencerStep  +  m_TimerCounterValue ) & 31;
            }
        }

        clk --;
    }

    return m_Amplitude;
}

uint32_t TriangleGenerator_t::TimerClock( void )
{
    uint16_t overflowCnt = 0;  // Counter overflow amount
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