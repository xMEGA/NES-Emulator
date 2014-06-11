/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _SquareGenerator_h_
#define _SquareGenerator_h_

#include <stdint.h>
#include "ApuRegisters.h"

class SquareGenerator_t
{
 public:

    void Init( uint8_t nChannel, uint32_t nsPerQuant );

    void WriteReg0( uint8_t value );
    void WriteReg1( uint8_t value );
    void WriteReg2( uint8_t value );
    void WriteReg3( uint8_t value );

    bool IsLengthCounter( void );    

    void EnvelopeUnitClock( void );
    void SweepUnitClock( void );
    void LengthCounterUnitClock( void );
    
    void Enable( void );
    void Disable( void );

    uint16_t GetQuant( void );

private:
    uint32_t TimerClock( void );
    uint8_t  SequencerClock( uint8_t clk );
    void     CalculateSweep( void );

private:
    uint8_t    m_Amplitude;        // Амплитуда сигнала
    
private:
    uint8_t    m_EnvelopeDivider; 
    uint8_t    m_EnvAmplitude;
    bool       m_EnvelopeStart;

private:
    uint8_t    m_SequencerStep;    // Состояние Sequencer-а  ( 0..7 )
    uint16_t   m_Period;            // Период сигнала
    int32_t    m_TimerCounterValue;    

private:
    uint8_t    m_SweepDivider; // Делитель
    bool       m_SweepReload;
    bool       m_IsSweepPeriodCorrect;

private:
    bool       m_LengthCounterUnitEnable;        
    uint8_t    m_LengthCounter; // Делитель

    uint8_t    m_ChannelNubmer;
    uint32_t   m_nsPerQuant;

private: // Registers
    ApuSquareControlReg_t m_ControlReg;
    ApuEnvelopeReg_t      m_EnvelopeReg;     
    ApuSquareSweepReg_t   m_SweepReg;
    ApuPeriodReg_t        m_PeriodReg;    
};

#endif