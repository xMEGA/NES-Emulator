/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _NoiseGenerator_h_
#define _NoiseGenerator_h_

#include <stdint.h>
#include "ApuRegisters.h"


class NoiseGenerator_t
{
public:
    void Init( uint32_t nsPerQuant );
     
    void WriteReg0( uint8_t value );
    void WriteReg1( uint8_t value );
    void WriteReg2( uint8_t value );

    void LengthCounterUnitClock( void );
    void EnvelopeUnitClock( void );

    void Enable( void );
    void Disable( void );



// -----------------------------------

    uint16_t GetQuant( void );

    uint32_t   m_nsPerQuant;


private:
    uint8_t  SequencerClock( uint8_t clk );
    uint32_t TimerClock( void );

private:    
    uint8_t    m_Amplitude;
    uint8_t    m_EnvelopeDivider; 
    uint8_t    m_EnvAmplitude;
    bool       m_EnvelopeStart;

private:    
    bool       m_LengthCounterUnitEnable;        
    uint8_t    m_LengthCounter; // Divider
    uint16_t   m_Period;
    uint16_t   m_ShiftRegister;

private:
    ApuNoiseControlReg_t m_ControlReg;
    ApuNoiseReg1_t       m_NoiseReg1;
    ApuEnvelopeReg_t     m_EnvelopeReg; 

private:
    int32_t    m_Timer;
};

#endif