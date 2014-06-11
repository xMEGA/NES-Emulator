/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _AudioDac_h_
#define _AudioDac_h_

#include <stdint.h>

#define _in_
#define _out_
#define _in_out_


#define AUDIO_DAC_SAMPLING_RATE   44100
#define AUDIO_DAC_BUFFER_SIZE     512


typedef void (*QueryFrameCallBack_t) ( _out_ void * pContext, _out_ int16_t* pData, _in_ uint16_t bytesCnt );

class AudioDac_t
{
public:

//    void (*QueryFrameCallBack)( _out_ void * context, _out_ int16_t* pData, _in_ uint16_t bytesCnt );


public:
    void SetQueryFrameCallBack( QueryFrameCallBack_t qeryFrameCallBack, void* pContext ); 
    void Init( void );
        

private:
    static void RequestFrameCallBack( void *pContext, uint8_t *audioStream, int length );
    
private:
    //SDL_AudioSpec         m_AudioSpec;
    QueryFrameCallBack_t  fp_QueryFrameCallBack;
    void*                 m_pContext;
};

#endif