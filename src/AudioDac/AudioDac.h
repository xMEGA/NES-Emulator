/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _AudioDac_h_
#define _AudioDac_h_

#include <stdint.h>

#define AUDIO_DAC_SAMPLING_RATE   44100
#define AUDIO_DAC_BUFFER_SIZE     1024

typedef void (*QueryFrameCallBack_t) ( void* pContext, int16_t* pData, uint16_t bytesCnt );

class AudioDac_t
{
public:
    void SetQueryFrameCallBack( QueryFrameCallBack_t qeryFrameCallBack, void* pContext ); 
    void Init( void );
        
private:
    static void RequestFrameCallBack( void *pContext, uint8_t *audioStream, int length );
    
private:
    QueryFrameCallBack_t  fp_QueryFrameCallBack;
    void*                 m_pContext;
};

#endif