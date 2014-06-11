/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Apu.h"

void Apu_t::SetInterruptRequestCallBack( void (*InterruptRequestCallBack)( _out_ void * pContext ), _in_ void * pContext )
{
    m_pContext         = pContext;
    IntRequestCallBack = InterruptRequestCallBack;
}

void Apu_t::SetAudioSamplingRate( uint32_t samplingRate )
{
    m_nsPerQuant = 1000000000 / samplingRate;
}


void Apu_t::Reset(void)
{
    ApuFrameSequencerReg.IrqDisable = APU_SET_BIT;
    m_FrameSequencerTime  = 0;
    m_FrameSequencerState = APU_FRAME_SEQUENCER_RESET_STATE;
    m_FrameSequencerIrqStatus = false;

    m_SquareCh1.Init( 1, m_nsPerQuant );
    m_SquareCh2.Init( 2, m_nsPerQuant );
    m_TriangleCh.Init( m_nsPerQuant );
    m_NoiseCh.Init( m_nsPerQuant );
}


void Apu_t::GetAudioFrame( int16_t* pData, uint16_t len )
{
    int16_t value = 0;
    int16_t squareOut;
    int16_t tndOut;

    for ( uint16_t i = 0; i != len; i++ ) // Подгружаем bytesCnt отсчетов в буфер аудио ЦАП
    {
        FrameSequencerRun( m_nsPerQuant );
    
        squareOut = /*752*/ 501 * m_SquareCh1.GetQuant() +  /*752*/ 501 * m_SquareCh2.GetQuant();

        tndOut = /*851*/ 567 * m_TriangleCh.GetQuant() + /*494*/ 329 * m_NoiseCh.GetQuant();// + 335 * dmc

        value = squareOut + tndOut;

        *pData ++= value;
    }
}



uint8_t Apu_t::Read( uint16_t addr )
{
    uint8_t retValue = 0;

    switch(addr)
    {
        case APU_STATUS_REG_ADDR:
                
            StatusReg.UpZeroLengthCtrSquareCh1  = APU_CLR_BIT;
            StatusReg.UpZeroLengthCtrSquareCh2  = APU_CLR_BIT;
            StatusReg.UpZeroLengthCtrTriangleCh = APU_CLR_BIT;
            StatusReg.UpZeroLengthCtrNoise      = APU_CLR_BIT;
            StatusReg.UpZeroLengthCtrDeltaMod   = APU_CLR_BIT;
            StatusReg.FrameIrqStatus = false;

            if( m_FrameSequencerIrqStatus )
            {
                StatusReg.FrameIrqStatus = true;
            }
    
            retValue = *(uint8_t *)&StatusReg;//m_SoundGenerator.ReadStatusReg();
        break;

        default: break;
    };
    return retValue;
}


void Apu_t::Write(uint16_t addr, uint8_t data)
{
    switch(addr)
    {
        case APU_CHANNEL_ENABLE_REG_ADDR:
            ChannelEnableReg.Value = data;
            ChannelEnableReg.EnableSquareCh1  ? m_SquareCh1.Enable()  : m_SquareCh1.Disable();
            ChannelEnableReg.EnableSquareCh2  ? m_SquareCh2.Enable()  : m_SquareCh2.Disable();
            ChannelEnableReg.EnableTriangleCh ? m_TriangleCh.Enable() : m_TriangleCh.Disable();
            ChannelEnableReg.EnableNoiseCh    ? m_NoiseCh.Enable()    : m_NoiseCh.Disable();
        break;

        case APU_FRAME_SEQUENCER_CONTROL_REG_ADDR:
            ApuFrameSequencerReg.Value = data;    
            m_FrameSequencerTime   = 0;
            m_FrameSequencerState  = APU_FRAME_SEQUENCER_RESET_STATE;
        break;
        
        // ---------------------- Channel 1 --------------------------
        case APU_REG_0_SQUARE_CHANNEL_1_ADDR:    
            m_SquareCh1.WriteReg0( data );
        break;

        case APU_REG_1_SQUARE_CHANNEL_1_ADDR:
            m_SquareCh1.WriteReg1( data );
        break;
                
        case APU_REG_2_SQUARE_CHANNEL_1_ADDR:
            m_SquareCh1.WriteReg2( data );
        break;

        case APU_REG_3_SQUARE_CHANNEL_1_ADDR:        
            m_SquareCh1.WriteReg3( data );                
        break;
        // ----------------------------------------------------------

        // ---------------------- Channel 2 --------------------------

        case APU_REG_0_SQUARE_CHANNEL_2_ADDR:        
            m_SquareCh2.WriteReg0( data );                    
        break;

        case APU_REG_1_SQUARE_CHANNEL_2_ADDR:
            m_SquareCh2.WriteReg1( data );        
        break;
                
        case APU_REG_2_SQUARE_CHANNEL_2_ADDR:    
            m_SquareCh2.WriteReg2( data );
        break;

        case APU_REG_3_SQUARE_CHANNEL_2_ADDR:                
            m_SquareCh2.WriteReg3( data );
        break;
        // ----------------------------------------------------------

        // ----------------- Triangle Channel ----------------------
        case APU_REG_0_TRIANGLE_CHANNEL_ADDR:
            m_TriangleCh.WriteReg0( data );
        break;
        
        case APU_REG_1_TRIANGLE_CHANNEL_ADDR:
            m_TriangleCh.WriteReg1( data );
        break;

        case APU_REG_2_TRIANGLE_CHANNEL_ADDR:
            m_TriangleCh.WriteReg2( data );
        break;
        // ----------------------------------------------------------


        // ------------------- Noise Channel -----------------------
        case APU_REG_0_NOISE_CHANNEL_ADDR:
            m_NoiseCh.WriteReg0( data );
        break;
        
        case APU_REG_1_NOISE_CHANNEL_ADDR:
            m_NoiseCh.WriteReg1( data );
        break;

        case APU_REG_2_NOISE_CHANNEL_ADDR:
            m_NoiseCh.WriteReg2( data );
        break;
        // ----------------------------------------------------------

        default: break;
    };
}

void Apu_t::FrameSequencerRun( uint32_t ns )
{
    m_FrameSequencerTime += ns;

    
    if( m_FrameSequencerTime >= APU_NS_PER_FRAME_SEQUENCER_CLOCK )
    {            

        if( 0 == ApuFrameSequencerReg.Mode )
        {
            if( m_FrameSequencerState == 4 )
            {
                m_FrameSequencerState = 0;
            }

            switch( m_FrameSequencerState )
            {
                case 0: 
                    m_SquareCh1.EnvelopeUnitClock();
                    m_SquareCh2.EnvelopeUnitClock();
                    m_TriangleCh.LinearCounterClock();

                    m_NoiseCh.EnvelopeUnitClock();
                break;
                
                case 1: 
                    m_SquareCh1.EnvelopeUnitClock();
                    m_SquareCh1.SweepUnitClock();
                    m_SquareCh1.LengthCounterUnitClock();
        
                    m_SquareCh2.EnvelopeUnitClock();
                    m_SquareCh2.SweepUnitClock();
                    m_SquareCh2.LengthCounterUnitClock();            

                    m_TriangleCh.LengthCounterUnitClock();
                    m_TriangleCh.LinearCounterClock();

                    m_NoiseCh.EnvelopeUnitClock();
                    m_NoiseCh.LengthCounterUnitClock();
                break;
        
                case 2: 
                    m_SquareCh1.EnvelopeUnitClock();
                    m_SquareCh2.EnvelopeUnitClock();

                    m_NoiseCh.EnvelopeUnitClock();

                    m_TriangleCh.LinearCounterClock();
                break;
        
                case 3: 
                    m_SquareCh1.EnvelopeUnitClock();
                    m_SquareCh1.SweepUnitClock();
                    m_SquareCh1.LengthCounterUnitClock();
                
                    m_SquareCh2.EnvelopeUnitClock();
                    m_SquareCh2.SweepUnitClock();
                    m_SquareCh2.LengthCounterUnitClock();
        
                    m_TriangleCh.LengthCounterUnitClock();
                    m_TriangleCh.LinearCounterClock();
        
                    m_NoiseCh.EnvelopeUnitClock();
                    m_NoiseCh.LengthCounterUnitClock();

                    m_FrameSequencerIrqStatus = true;
                   
                    if( 0 == ApuFrameSequencerReg.IrqDisable )
                    {
                       IntRequestCallBack( m_pContext );
                    }                
                    // IRQ
                break;
        
                default:
                
                break;

            }
        }
        else
        {
            if( m_FrameSequencerState == 5 )
            {
                m_FrameSequencerState = 0;
            }

            switch( m_FrameSequencerState )
            {
                case 0: 
                    m_SquareCh1.EnvelopeUnitClock();
                    m_SquareCh1.SweepUnitClock();
                    m_SquareCh1.LengthCounterUnitClock();
            
                    m_SquareCh2.EnvelopeUnitClock();
                    m_SquareCh2.SweepUnitClock();
                    m_SquareCh2.LengthCounterUnitClock();

                    m_TriangleCh.LengthCounterUnitClock();
                    m_TriangleCh.LinearCounterClock();

                    m_NoiseCh.EnvelopeUnitClock();
                    m_NoiseCh.LengthCounterUnitClock();
                
                break;
                
                case 1: 
                    m_SquareCh1.EnvelopeUnitClock();
                    m_SquareCh2.EnvelopeUnitClock();
                    m_TriangleCh.LinearCounterClock();

                    m_NoiseCh.EnvelopeUnitClock();
        
                break;
        
                case 2: 
                    m_SquareCh1.EnvelopeUnitClock();
                    m_SquareCh1.SweepUnitClock();
                    m_SquareCh1.LengthCounterUnitClock();

                    m_SquareCh2.EnvelopeUnitClock();
                    m_SquareCh2.SweepUnitClock();
                    m_SquareCh2.LengthCounterUnitClock();

                    m_TriangleCh.LengthCounterUnitClock();
                    m_TriangleCh.LinearCounterClock();

                    m_NoiseCh.EnvelopeUnitClock();
                    m_NoiseCh.LengthCounterUnitClock();

                break;
        
                case 3: 
                    m_SquareCh1.EnvelopeUnitClock();
                    m_SquareCh2.EnvelopeUnitClock();
                    m_TriangleCh.LinearCounterClock();

                    m_NoiseCh.EnvelopeUnitClock();
                break;

                default:
                
                break;

            }
        }
          m_FrameSequencerTime = 0;
        m_FrameSequencerState ++;
    }
}