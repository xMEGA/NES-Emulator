/*
    NES Emulator Project
    Created by Igor Belorus
    07/23/2015
*/

#include "TimeCounter.h"
#include "SDL.h"

//    SDL_Init( SDL_INIT_TIMER );

uint32_t TimeCounterGetMsec()
{
	uint64_t perfCounter = SDL_GetPerformanceCounter();
    uint64_t perfFreq = SDL_GetPerformanceFrequency();

    uint32_t msec = ( uint32_t )( 1000000 * perfCounter / perfFreq );
    
	return msec;
}