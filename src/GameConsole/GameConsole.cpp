/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "GameConsole.h"


void GameConsole_t::SaveGameContext( uint8_t* pData )
{

}


void GameConsole_t::LoadGameContext( uint8_t* pData )
{


}

void GameConsole_t::SetAudioSamplingRate( uint32_t samplingRate )
{
    m_Apu.SetAudioSamplingRate( samplingRate );
}

void GameConsole_t::GetAudioFrame( int16_t* pData, uint16_t len )
{
    m_Apu.GetAudioFrame( pData, len );
}

uint16_t GameConsole_t::GetFramesPerSecond( void )
{
    return m_FramesPerSecond;
}

void GameConsole_t::SetButtonJoysticA(uint8_t button)
{
    m_Control.SetButtonJoysticA( button );
}

void GameConsole_t::SetButtonJoysticB(uint8_t button)
{
    m_Control.SetButtonJoysticB( button );
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

void GameConsole_t::Init(void)
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


void GameConsole_t::Run( uint32_t sysTick )
{

    if( ( sysTick - m_FramesLastSysTick ) >= USECOND_IN_SECOND )
    {
        m_FramesLastSysTick = sysTick;
        m_FramesPerSecond = m_FramesCnt;
        m_FramesCnt = 0;
    }

    uint32_t deltaTime = sysTick - m_LastSysTick;
	
    if ( deltaTime > ONE_FRAME_TIME )
    {
        m_LastSysTick = sysTick; 

        m_CpuCycles = 0;

        uint32_t ppuCycles = 0;

        do
        {
            uint32_t cycles = m_Cpu.Run();
            ppuCycles += m_Ppu.Run( cycles );            
        }
        while( ( ppuCycles < 89342  ) );
        
        m_FramesCnt++;
    }

}

void GameConsole_t::CpuBusWrite( void * pContext, uint16_t busAddr, uint8_t busData)
{
    GameConsole_t* gameConsole = static_cast<GameConsole_t *>(pContext);

    uint8_t readData = 0;
    

    if( busAddr < 0x2000 )
    {
        busAddr &= 0x07FF;
        gameConsole->m_Ram[ busAddr ] = busData;
    }
    else if( busAddr < 0x2008 )
    {
        gameConsole->m_Ppu.Write(busAddr, busData);
    }
    else if( busAddr <= 0x4017 )
    {
    
        if( DMA_REG_CPU_ADDR == busAddr ) // DMA
        {
            uint16_t intMemAddr = busData << 8;

            gameConsole->m_Ppu.Write( PPU_REG_ADDR_SPRITES_ADDR, 0 );

            //cpuResCycles -= 2 * 256;
            gameConsole->m_CpuCycles  += 2 * 256;


            for( uint16_t i = 0; i != 256; i++ )
            {
                readData = gameConsole->m_Ram[ intMemAddr ];
                gameConsole->m_Ppu.Write( PPU_REG_DATA_SPRITES_ADDR, readData );
                intMemAddr++;
            }

        }
        else
        {
            gameConsole->m_Apu.Write(busAddr, busData);
            gameConsole->m_Control.Write(busAddr, busData);
        }

    }
    else //if( busAddr <= 0xBFFF)
    {
        gameConsole->m_Cartridge.CpuWrite(busAddr, busData);
    }
}

uint8_t GameConsole_t::CpuBusRead( void * pContext, uint16_t busAddr )
{
    GameConsole_t* gameConsole = static_cast<GameConsole_t *>(pContext);

    uint8_t readData = 0;

    
    if( busAddr < 0x2000)
    {
        busAddr &= 0x07FF; // CPU RAM mirroring
        readData = gameConsole->m_Ram[ busAddr ];
    }
    else if( busAddr <= 0x2FFA )
    {

        readData = gameConsole->m_Ppu.Read(busAddr);
    }
    else if( busAddr <= 0x4017 )
    {
        readData = gameConsole->m_Apu.Read( busAddr );

        if( ( busAddr == 0x4016) || ( busAddr == 0x4017) ) 
        {
            readData = gameConsole->m_Control.Read( busAddr );
        }
    }
    else
    {
        readData = gameConsole->m_Cartridge.CpuRead( busAddr );
    }

    return readData;
}

void GameConsole_t::PpuBusWrite( void * pContext, uint16_t busAddr, uint8_t busData)
{
    GameConsole_t* gameConsole = static_cast<GameConsole_t *>(pContext);
    busAddr &= 0x3FFF;
    gameConsole->m_Cartridge.PpuWrite( busAddr, busData );
}


uint8_t GameConsole_t::PpuBusRead( void * pContext, uint16_t busAddr )
{
    GameConsole_t* gameConsole = static_cast<GameConsole_t *>(pContext);
    uint8_t data;

    busAddr &= 0x3FFF;//!!!

    data = gameConsole->m_Cartridge.PpuRead( busAddr );

    return data;
}

void GameConsole_t::CartridgeInterruptReqest( _out_ void * pContext )
{
    GameConsole_t* gameConsole = static_cast<GameConsole_t *>(pContext);
    gameConsole->m_Cpu.InterruptRequest();
}

void GameConsole_t::ApuInterruptReqest( _out_ void * pContext )
{
    GameConsole_t* gameConsole = static_cast<GameConsole_t *>(pContext);
    gameConsole->m_Cpu.InterruptRequest();
}

void GameConsole_t::VsyncSignal( _out_ void * pContext )
{
    GameConsole_t* gameConsole = static_cast<GameConsole_t *>(pContext);
    gameConsole->m_Cpu.NonMaskableInterrupt();
}