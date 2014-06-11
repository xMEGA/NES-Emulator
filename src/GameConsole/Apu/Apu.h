/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _Apu_h_
#define _Apu_h_

#include <stdint.h>
#include "ApuRegisters.h"
#include "SquareGenerator.h"
#include "TriangleGenerator.h"
#include "NoiseGenerator.h"

#define _in_
#define _out_
#define _in_out_

#define APU_SET_BIT 1
#define APU_CLR_BIT 0

#define APU_FRAME_SEQUENCE_CLOCK_FREQ       240
#define APU_NS_PER_FRAME_SEQUENCER_CLOCK    1000000000 / APU_FRAME_SEQUENCE_CLOCK_FREQ // 240 Hz in ns
#define APU_FRAME_SEQUENCER_RESET_STATE     1

class Apu_t
{
public:
    void Reset(void);
    void SetInterruptRequestCallBack( void (*InterruptRequestCallBack)( _out_ void * pContext ), _in_ void * pContext );
        
    // --- Интерфейс взаимодействия с процессором ---
    uint8_t Read( uint16_t addr );
    void Write( uint16_t addr, uint8_t data );
    void SetAudioSamplingRate( uint32_t samplingRate );
    // ----------------------------------------------

    void  GetAudioFrame( int16_t* pData, uint16_t len );
                
private:
    void FrameSequencerRun( uint32_t ns );

private: // Каналы
    SquareGenerator_t   m_SquareCh1;
    SquareGenerator_t   m_SquareCh2;
    TriangleGenerator_t m_TriangleCh;
    NoiseGenerator_t    m_NoiseCh;

private: // Регистры, общие для всего APU
    ApuChannelEnableReg_t    ChannelEnableReg;
    ApuFrameSequencerReg_t   ApuFrameSequencerReg;
    ApuStatusReg_t           StatusReg;

private:
    uint32_t    m_FrameSequencerTime;
    uint8_t     m_FrameSequencerState;
    bool        m_FrameSequencerIrqStatus;
    uint32_t    m_nsPerQuant;

private:
    void* m_pContext;
    void (*IntRequestCallBack)( _out_ void * pContext );
};

#endif