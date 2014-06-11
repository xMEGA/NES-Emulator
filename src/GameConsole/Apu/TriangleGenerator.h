/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _TriangleGenerator_h_
#define _TriangleGenerator_h_

#include <stdint.h>
#include "ApuRegisters.h"


class TriangleGenerator_t
{
public:
    void Init( uint32_t nsPerQuant );

    void WriteReg0( uint8_t value );
    void WriteReg1( uint8_t value );
    void WriteReg2( uint8_t value );

    void LinearCounterClock( void );
    void LengthCounterUnitClock( void );
    
    void Enable( void );
    void Disable( void );

    uint16_t GetQuant( void );


private:
    uint8_t    SequencerClock( uint8_t clk );
    uint32_t   TimerClock( void );

private:    
    bool                m_LengthCounterUnitEnable;
    uint8_t             m_LengthCounter; // Divider
    bool                m_LinerCounterHalt;
    uint8_t             m_LinearCounter;
    uint32_t            m_nsPerQuant;

private:
    ApuPeriodReg_t      m_PeriodReg;
    ApuTriangleReg0_t   m_TriangleReg0;
    int32_t             m_TimerCounterValue;    
    uint8_t             m_Amplitude;
    uint8_t             m_SequencerStep;
};

#endif