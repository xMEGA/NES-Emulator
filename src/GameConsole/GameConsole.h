/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _GameConsole_h_
#define _GameConsole_h_

#include <stdint.h>
#include "Cpu/Cpu.h"
#include "Cartridge/Cartridge.h"
#include "Ppu/Ppu.h"
#include "Control/Control.h"
#include "Apu/Apu.h"

#define GAME_CONTEXT_SIZE           10000

#define DMA_REG_CPU_ADDR            0x4014


//#define CPU_FREQ                    1789772  // Частота процессора в Гц (NTSC)
//#define CPU_FREQ                    1773447  // Частота процессора в Гц (PAL)
//#define SCREEN_REFRESH_RATE         60       // Частота обновления экрана в Гц


//#define CPU_PERIOD_NS              (uint32_t) (1000000000 / CPU_FREQ)
//#define SCREEN_REFRESH_PERIOD_NS   (uint32_t) (1000000000 / SCREEN_REFRESH_RATE)
//#define FRAME_CYCLES               (uint32_t) (SCREEN_REFRESH_PERIOD_NS / CPU_PERIOD_NS)


//#define INTERNAL_RAM_BASE_CPU_ADDR  0x0000






#define USECOND_IN_SECOND          1000000
#define NTSC_FPS				   60
#define ONE_FRAME_TIME             USECOND_IN_SECOND / NTSC_FPS


#define INTERNAL_RAM_SIZE           0x0800

typedef void ( *PresentFrameCallBack_t )     ( void * context, uint8_t* pData, uint16_t len, uint16_t posInFrame );
typedef void ( *RomFileAccesCallBack_t )     ( void * context, uint8_t* pData, uint32_t offset, uint16_t bytesCnt );

class GameConsole_t
{

public:
    void SetPresentFrameCallBack( PresentFrameCallBack_t presentFrameCallBack, void * pContext );
    void SetRomFileAccesCallBack( RomFileAccesCallBack_t romFileAccesCallBack, void * pContext );
        
    void Init();
    void SetAudioSamplingRate( uint32_t samplingRate );
    
    //  ------ For Save Games ---------------
    void SaveGameContext( uint8_t* pData );
    void LoadGameContext( uint8_t* pData );
    // --------------------------------------

    void Run( uint32_t sysTick );
    void SetButtonGamepadA( uint8_t button );
    void SetButtonGamepadB( uint8_t button );   
    void GetAudioFrame( int16_t* pData, uint16_t len );
    uint16_t GetFramesPerSecond();
    
    void DumpPpuMemory( uint8_t* pDestBuffer, uint16_t startAddr, uint16_t size );
    void DumpCpuMemory( uint8_t* pDestBuffer, uint16_t startAddr, uint16_t size );
    void DumpCpuRegisters( uint8_t* pOutRegistersData );
    
private:
    static void    CpuBusWrite( void* pContext, uint16_t busAddr, uint8_t busData );
    static uint8_t CpuBusRead ( void* pContext, uint16_t busAddr );
    static void    PpuBusWrite( void* pContext, uint16_t busAddr, uint8_t busData );
    static uint8_t PpuBusRead ( void* pContext, uint16_t busAddr );

    static void    VsyncSignal( void* pContext );
    static void    CartridgeInterruptReqest( void* pContext );
    static void    ApuInterruptReqest( void* pContext );

    static void    CartridgeIrqCallBack( void* pContext );


private:
    Cpu_t          m_Cpu;
    Ppu_t          m_Ppu;         
    Control_t      m_Control;
    Apu_t          m_Apu;
    Cartridge_t    m_Cartridge;
    uint8_t        m_Ram[ INTERNAL_RAM_SIZE ];        

private:
    uint32_t       m_LastSysTick;
    uint16_t       m_FramesPerSecond;
    uint16_t       m_FramesCnt;
    uint32_t       m_FramesLastSysTick;
    uint16_t       m_CpuCycles;

private:
    PresentFrameCallBack_t fp_PresentFrameCallBack;
    RomFileAccesCallBack_t fp_RomFileAccesCallBack;
    void*                  m_pContext;
};


#endif