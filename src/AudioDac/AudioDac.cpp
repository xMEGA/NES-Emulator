/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include  "AudioDac.h"
#include "SDL.h" 

void AudioDac_t::SetQueryFrameCallBack( QueryFrameCallBack_t qeryFrameCallBack, void* pContext )
{
    fp_QueryFrameCallBack = qeryFrameCallBack;
    m_pContext = pContext;
}


void AudioDac_t::Init( void )
{
    SDL_Init( SDL_INIT_AUDIO );

    SDL_AudioSpec         m_AudioSpec;
    
    m_AudioSpec.freq     = AUDIO_DAC_SAMPLING_RATE;
    m_AudioSpec.format   = AUDIO_S16LSB;
    m_AudioSpec.channels = 1;
    m_AudioSpec.samples  = AUDIO_DAC_BUFFER_SIZE;
    m_AudioSpec.callback = RequestFrameCallBack;
    m_AudioSpec.userdata = this;
    
    SDL_OpenAudio( &m_AudioSpec, NULL );
    SDL_PauseAudio(0);  // start play audio
}


void AudioDac_t::RequestFrameCallBack( void *pContext, uint8_t *audioStream, int length )
{
    int16_t *stream = reinterpret_cast< int16_t* > ( audioStream);
    AudioDac_t* pAudioDac = static_cast< AudioDac_t* >( pContext );

    length /= sizeof( int16_t ); // Количество выборок
    
    pAudioDac->fp_QueryFrameCallBack( pAudioDac->m_pContext, stream, length);
}