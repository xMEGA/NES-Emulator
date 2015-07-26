/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "GameConsole.h"

uint32_t GameConsole_t::SaveGameContext( uint8_t* pData, uint32_t len  )
{
    uint8_t* pInputData = pData;

    pData += m_Cpu.DumpRegisters( pData );
    pData += m_Cpu.DumpMemory( pData, 0, 0x800 );
        
    return pData - pInputData;
}


void GameConsole_t::LoadGameContext( uint8_t* pData, uint32_t len )
{
    pData += m_Cpu.LoadRegisters( pData );
    pData += m_Cpu.LoadToMemory( pData, 0, 0x800 );
}

void GameConsole_t::DumpPpuMemory( uint8_t* pDestBuffer, uint16_t startAddr, uint16_t size )
{
    for( uint16_t addr = startAddr; addr < ( startAddr + size ); addr++ )
    {
        *pDestBuffer ++= m_Cartridge.PpuRead( addr );
    }
}

void GameConsole_t::DumpCpuMemory( uint8_t* pDestBuffer, uint16_t startAddr, uint16_t size )
{
    m_Cpu.DumpMemory( pDestBuffer, startAddr, size );
}

void GameConsole_t::DumpCpuRegisters( uint8_t* pOutRegistersData )
{
    m_Cpu.DumpRegisters( pOutRegistersData );
}

void GameConsole_t::SetAudioSamplingRate( uint32_t samplingRate )
{
    m_Apu.SetAudioSamplingRate( samplingRate );
}

void GameConsole_t::GetAudioFrame( int16_t* pData, uint16_t len )
{
    m_Apu.GetAudioFrame( pData, len );
}

uint16_t GameConsole_t::GetFramesPerSecond()
{
    return m_FramesPerSecond;
}

void GameConsole_t::SetButtonGamepadA(uint8_t button)
{
    m_Control.SetButtonGamepadA( button );
}

void GameConsole_t::SetButtonGamepadB(uint8_t button)
{
    m_Control.SetButtonGamepadB( button );
}

void GameConsole_t::SetPresentFrameCallBack( PresentFrameCallBack_t presentFrameCallBack, void * pContext )
{
    fp_PresentFrameCallBack = presentFrameCallBack;
    m_pContext = pContext;
}


void GameConsole_t::SetRomFileAccesCallBack( RomFileAccesCallBack_t romFileAccesCallBack, void * pContext )
{
    fp_RomFileAccesCallBack = romFileAccesCallBack;
    m_pContext = pContext;
}

void GameConsole_t::Init()
{
    m_Cartridge.SetRomFileAccesCallBack( fp_RomFileAccesCallBack, m_pContext );
    m_Cartridge.SetInterruptRequestCallBack( CartridgeInterruptReqest, m_pContext );
    m_Cartridge.Reset();

    m_Cpu.SetBusReadCallBack( CpuBusRead, this );
    m_Cpu.SetBusWriteCallBack( CpuBusWrite, this ); 
    m_Cpu.Reset();

    m_Ppu.SetPresentFrameCallBack( fp_PresentFrameCallBack, m_pContext );
    m_Ppu.SetBusWriteCallBack( PpuBusWrite, this );
    m_Ppu.SetBusReadCallBack( PpuBusRead, this );
    m_Ppu.SetVsyncSignalCallBack( VsyncSignal, this );
    m_Ppu.SetLineCounterCallBack( CartridgeIrqCallBack, this );    
    m_Ppu.Reset();

    m_Apu.SetInterruptRequestCallBack( ApuInterruptReqest, this );
    m_Apu.Reset();

    m_Control.Reset();

    m_LastSysTick = 0;
    m_FramesPerSecond = 0;
    m_FramesCnt = 0;
    m_FramesLastSysTick = 0;
    m_CpuCycles = 0;
}

void GameConsole_t::CartridgeIrqCallBack( void* pContext )
{
    GameConsole_t* gameConsole = static_cast<GameConsole_t *>(pContext);
    gameConsole->m_Cartridge.IrqCounterDecrement();
}


void GameConsole_t::ProcessingOneFrame( uint32_t sysTick )
{

    if( ( sysTick - m_FramesLastSysTick ) >= USECOND_IN_SECOND )
    {
        m_FramesLastSysTick = sysTick;
        m_FramesPerSecond = m_FramesCnt;
        m_FramesCnt = 0;
    }

    uint32_t deltaTime = sysTick - m_LastSysTick;
	
    //if ( deltaTime > ONE_FRAME_TIME )
    {
        m_LastSysTick = sysTick; 

        m_CpuCycles = 0;

        uint32_t ppuCycles = 0;

        do
        {
            uint32_t cycles = m_Cpu.ExecuteOneCommand();
            ppuCycles += m_Ppu.Run( cycles );            
        }
        while( ppuCycles < 89342 );
        
        m_FramesCnt++;
    }

}

void GameConsole_t::CpuBusWrite( void * pContext, uint16_t busAddr, uint8_t busData)
{
    GameConsole_t* pGameConsole = static_cast<GameConsole_t *>(pContext);

    uint8_t readData = 0;
    

    if( busAddr < 0x2000 )
    {
        busAddr &= 0x07FF;
        pGameConsole->m_Ram[ busAddr ] = busData;
    }
    else if( busAddr < 0x2008 )
    {
        pGameConsole->m_Ppu.Write(busAddr, busData);
    }
    else if( busAddr <= 0x4017 )
    {
    
        if( DMA_REG_CPU_ADDR == busAddr ) // DMA
        {
            uint16_t intMemAddr = busData << 8;

            pGameConsole->m_Ppu.Write( PPU_REG_ADDR_SPRITES_ADDR, 0 );

            //cpuResCycles -= 2 * 256;
            pGameConsole->m_CpuCycles  += 2 * 256;


            for( uint16_t i = 0; i != 256; i++ )
            {
                readData = pGameConsole->m_Ram[ intMemAddr ];
                pGameConsole->m_Ppu.Write( PPU_REG_DATA_SPRITES_ADDR, readData );
                intMemAddr++;
            }

        }
        else
        {
            pGameConsole->m_Apu.Write(busAddr, busData);
            pGameConsole->m_Control.Write(busAddr, busData);
        }

    }
    else //if( busAddr <= 0xBFFF)
    {
        pGameConsole->m_Cartridge.CpuWrite(busAddr, busData);
    }
}

uint8_t GameConsole_t::CpuBusRead( void * pContext, uint16_t busAddr )
{
    GameConsole_t* pGameConsole = static_cast<GameConsole_t *>(pContext);

    uint8_t readData = 0;

    
    if( busAddr < 0x2000)
    {
        busAddr &= 0x07FF; // CPU RAM mirroring
        readData = pGameConsole->m_Ram[ busAddr ];
    }
    else if( busAddr <= 0x2FFA )
    {

        readData = pGameConsole->m_Ppu.Read(busAddr);
    }
    else if( busAddr <= 0x4017 )
    {
        readData = pGameConsole->m_Apu.Read( busAddr );

        if( ( busAddr == 0x4016) || ( busAddr == 0x4017) ) 
        {
            readData = pGameConsole->m_Control.Read( busAddr );
        }
    }
    else
    {
        readData = pGameConsole->m_Cartridge.CpuRead( busAddr );
    }

    return readData;
}

void GameConsole_t::PpuBusWrite( void * pContext, uint16_t busAddr, uint8_t busData)
{
    GameConsole_t* pGameConsole = static_cast<GameConsole_t *>(pContext);
    busAddr &= 0x3FFF;
    pGameConsole->m_Cartridge.PpuWrite( busAddr, busData );
}


uint8_t GameConsole_t::PpuBusRead( void * pContext, uint16_t busAddr )
{
    GameConsole_t* pGameConsole = static_cast<GameConsole_t *>(pContext);
    uint8_t data;

    busAddr &= 0x3FFF;//!!!

    data = pGameConsole->m_Cartridge.PpuRead( busAddr );

    return data;
}

void GameConsole_t::CartridgeInterruptReqest( void * pContext )
{
    GameConsole_t* pGameConsole = static_cast<GameConsole_t *>(pContext);
    pGameConsole->m_Cpu.InterruptRequest();
}

void GameConsole_t::ApuInterruptReqest( void * pContext )
{
    GameConsole_t* pGameConsole = static_cast<GameConsole_t *>(pContext);
    pGameConsole->m_Cpu.InterruptRequest();
}

void GameConsole_t::VsyncSignal( void * pContext )
{
    GameConsole_t* pGameConsole = static_cast<GameConsole_t *>(pContext);
    pGameConsole->m_Cpu.NonMaskableInterrupt();
}