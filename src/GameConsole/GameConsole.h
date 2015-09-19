/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _GameConsole_h_
#define _GameConsole_h_

#include <stdint.h>
#include "Cpu/Cpu.h"
#include "Ppu/Ppu.h"
#include "Control/Control.h"
#include "Cartridge/Mappers/IMapper.h"
#include "Apu/Apu.h"


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

typedef void ( *PresentScanLineCallBack_t )     ( void* pContext, uint8_t* pData, uint16_t scanLine );

class GameConsole_t
{

public:
    void SetPresentScanLineCallBack( PresentScanLineCallBack_t presentFrameCallBack, void * pContext );
    void LoadGameRomFile( uint8_t* pData, uint16_t size );
    
    void Init();
    void SetAudioSamplingRate( uint32_t samplingRate );
    
    //  ------ For Save Games ---------------
    uint32_t SaveGameContext( uint8_t* pData, uint32_t len );
    void LoadGameContext( uint8_t* pData, uint32_t len );
    // --------------------------------------

    void ProcessingOneFrame( uint64_t sysTick );
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
    inline void     CpuBusWrite( uint16_t busAddr, uint8_t busData );
    inline uint8_t  CpuBusRead ( uint16_t busAddr );
    inline void     PpuBusWrite( uint16_t busAddr, uint8_t busData );
    inline uint8_t  PpuBusRead ( uint16_t busAddr );
    inline uint16_t TranslateVideoRamAddr( uint16_t addr );
    IMapper_t* CreateMapper( CartridgeMapperType_t type );

private:
    Cpu_t          m_Cpu;
    Ppu_t          m_Ppu;         
    Control_t      m_Control;
    Apu_t          m_Apu;

    uint8_t        m_Ram[ INTERNAL_RAM_SIZE ];
    uint8_t        m_CartrigeChrRam   [ 0x2000 ];
    uint8_t        m_CartrigeVideoRam [ 0x1000 ];
    uint8_t        m_CartridgeRam     [ 0x2000 ];
    IMapper_t*     m_pMapper;
    uint8_t*                    m_pGameRomFile;
    GameRomInfo_t  m_GameRomInfo;
    
    uint8_t*       m_pRomFileBuffer;
    
private:
    uint64_t       m_LastSysTick;
    uint16_t       m_FramesPerSecond;
    uint16_t       m_FramesCnt;
    uint64_t       m_FramesLastSysTick;
    uint16_t       m_CpuCycles;

private:
    PresentScanLineCallBack_t m_fpPresentScanLineCallBack;
    RomFileAccesCallBack_t    m_fpRomFileAccesCallBack;
    void*                     m_pContext;
};


#endif
