/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "GameConsole.h"
#include <stddef.h>
#include <string.h>
#include "Cartridge/Mappers/NoRom.h"
#include "Cartridge/Mappers/UnRom.h"
#include "Cartridge/Mappers/AoRom.h"
#include "Cartridge/Mappers/Mmc3.h"
#include "Cartridge/Mappers/Mmc1.h"

IMapper_t* GameConsole_t::CreateMapper( CartridgeMapperType_t type )
{
    IMapper_t* pIMapper = NULL;
    
    switch( type )
    {
        case NO_MAPPER_MAPPER_TYPE:
            pIMapper = new NO_Mapper_t;
        break;

        case NINTENDO_MMC1_MAPPER_TYPE:
            pIMapper = new MMC1_Mapper_t;
        break;

        case UNROM_MAPPER_TYPE:
            pIMapper = new UNROM_Mapper_t;
        break;

        case NINTENDO_MMC3_MAPPER_TYPE:
           pIMapper = new MMC3_Mapper_t;
        break;

        case AOROM_MAPPER_TYPE:
           pIMapper = new AOROM_Mapper_t;
        break;

    default:

    break;
                
    }
    
    return pIMapper;
}

void GameConsole_t::LoadGameRomFile( uint8_t* pData, uint16_t size )
{
    UNUSED( size );

    m_pRomFileBuffer = pData;

    //m_Cartridge.Reset( pData, size, CartridgeInterruptReqest, m_pContext );
    
    m_pGameRomFile = pData;
    
    uint8_t gameRomInfoBuffer[ ROM_FILE_HEADER_SIZE ];
    MapperInfo_t mapperInfo;

    memcpy( gameRomInfoBuffer, m_pGameRomFile, ROM_FILE_HEADER_SIZE );
    
    GameRomAnaliser_t gameRomAnaliser;
    
    m_GameRomInfo = gameRomAnaliser.GetGameRomInfo( gameRomInfoBuffer );
    
    mapperInfo.CartridgeMapperType = m_GameRomInfo.CartridgeMapperType;
    mapperInfo.MirroringType       = m_GameRomInfo.MirroringType;
    mapperInfo.NumberOfRomBanks    = m_GameRomInfo.NumberOfRomBanks;

    
    if( NULL != m_pMapper )
    {
        delete m_pMapper;
        m_pMapper = NULL;
    }
      
    m_pMapper = CreateMapper( mapperInfo.CartridgeMapperType );
    
    if( NULL != m_pMapper )
    {
        m_pMapper->Init( mapperInfo );
        m_pMapper->SetInterruptRequestCallBack( CartridgeInterruptReqest, this );
    }
    
}


uint32_t GameConsole_t::SaveGameContext( uint8_t* pData, uint32_t len  )
{
    UNUSED( len );

    uint8_t* pInputData = pData;

    pData += m_Cpu.DumpRegisters( pData );
    pData += m_Cpu.DumpMemory( pData, 0, 0x800 );
        
    return pData - pInputData;
}


void GameConsole_t::LoadGameContext( uint8_t* pData, uint32_t len )
{
    UNUSED( len );

    pData += m_Cpu.LoadRegisters( pData );
    pData += m_Cpu.LoadToMemory( pData, 0, 0x800 );
}

void GameConsole_t::DumpPpuMemory( uint8_t* pDestBuffer, uint16_t startAddr, uint16_t size )
{
    UNUSED( pDestBuffer );

    for( uint16_t addr = startAddr; addr < ( startAddr + size ); addr++ )
    {
//        *pDestBuffer ++= m_Cartridge.PpuRead( addr );
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

void GameConsole_t::SetPresentScanLineCallBack( PresentScanLineCallBack_t presentFrameCallBack, void * pContext )
{
    m_fpPresentScanLineCallBack = presentFrameCallBack;
    m_pContext = pContext;
}

void GameConsole_t::Init()
{
    m_Cpu.SetBusReadCallBack( CpuBusRead, this );
    m_Cpu.SetBusWriteCallBack( CpuBusWrite, this ); 
    m_Cpu.Reset();

    m_Ppu.SetPresentFrameCallBack( m_fpPresentScanLineCallBack, m_pContext );
    m_Ppu.SetBusWriteCallBack( PpuBusWrite, this );
    m_Ppu.SetBusReadCallBack( PpuBusRead, this );
    m_Ppu.SetVsyncSignalCallBack( VsyncSignal, this );
    m_Ppu.SetLineCounterCallBack( CartridgeIrqCallBack, this );    
    m_Ppu.Reset();

    m_Apu.SetInterruptRequestCallBack( ApuInterruptReqest, this );
    m_Apu.Reset();

    m_Control.Reset();

    m_LastSysTick       = 0;
    m_FramesPerSecond   = 0;
    m_FramesCnt         = 0;
    m_FramesLastSysTick = 0;
    m_CpuCycles         = 0;
    m_PpuCycles         = 0;
    m_pMapper           = NULL;
}

void GameConsole_t::CartridgeIrqCallBack( void* pContext )
{
    GameConsole_t* pGameConsole = static_cast< GameConsole_t* >( pContext );
    pGameConsole->m_pMapper->IrqCounterDecrement();
}

void GameConsole_t::ProcessingOneFrame( uint64_t sysTick )
{

    if( ( sysTick - m_FramesLastSysTick ) >= USECOND_IN_SECOND )
    {
        m_FramesLastSysTick = sysTick;
        m_FramesPerSecond = m_FramesCnt;
        m_FramesCnt = 0;
    }

    uint64_t deltaTime = sysTick - m_LastSysTick;


    if ( deltaTime > ONE_FRAME_TIME )
    {
        m_LastSysTick = sysTick;

                
        do
        {
            m_CpuCycles = 0;
        
            m_CpuCycles += m_Cpu.ExecuteOneCommand();
            uint32_t ppuCycles = m_Ppu.Run( m_CpuCycles );

            m_PpuCycles += ppuCycles;
        }
        while( m_PpuCycles < 89342 );
       
        m_PpuCycles -= 89342;

        m_FramesCnt++;
    }
}

void GameConsole_t::CpuBusWrite( void* pContext, uint16_t busAddr, uint8_t busData )
{
    GameConsole_t* pGameConsole = static_cast< GameConsole_t* >( pContext );
    pGameConsole->CpuBusWrite( busAddr, busData );
}

uint8_t GameConsole_t::CpuBusRead( void* pContext, uint16_t busAddr )
{
    GameConsole_t* pGameConsole = static_cast< GameConsole_t* >( pContext );
    return pGameConsole->CpuBusRead( busAddr );
}

void GameConsole_t::PpuBusWrite( void* pContext, uint16_t busAddr, uint8_t busData )
{
    GameConsole_t* pGameConsole = static_cast< GameConsole_t* >( pContext );
    pGameConsole->PpuBusWrite( busAddr, busData );
}

uint8_t GameConsole_t::PpuBusRead( void* pContext, uint16_t busAddr )
{
    GameConsole_t* pGameConsole = static_cast< GameConsole_t* >( pContext );
    return pGameConsole->PpuBusRead( busAddr );
}

void GameConsole_t::CartridgeInterruptReqest( void* pContext )
{
    GameConsole_t* pGameConsole = static_cast< GameConsole_t* >( pContext );
    pGameConsole->m_Cpu.InterruptRequest();
}

void GameConsole_t::ApuInterruptReqest( void* pContext )
{
    GameConsole_t* pGameConsole = static_cast< GameConsole_t* >( pContext );
    pGameConsole->m_Cpu.InterruptRequest();
}

void GameConsole_t::VsyncSignal( void* pContext )
{
    GameConsole_t* pGameConsole = static_cast< GameConsole_t* >( pContext );
    pGameConsole->m_Cpu.NonMaskableInterrupt();
}

void GameConsole_t::CpuBusWrite( uint16_t busAddr, uint8_t busData)
{
    uint8_t readData = 0;
    

    if( busAddr < 0x2000 )
    {
        busAddr &= 0x07FF;
        m_Ram[ busAddr ] = busData;
    }
    else if( busAddr < 0x2008 )
    {
        m_Ppu.Write(busAddr, busData);
    }
    else if( busAddr <= 0x4017 )
    {
    
        if( DMA_REG_CPU_ADDR == busAddr ) // DMA
        {
            uint16_t intMemAddr = busData << 8;

            m_Ppu.Write( PPU_REG_ADDR_SPRITES_ADDR, 0 );

            //cpuResCycles -= 2 * 256;
            m_CpuCycles  += 2 * 256;


            for( uint16_t idx = 0; idx < 256; idx++ )
            {
                readData = m_Ram[ intMemAddr ];
                m_Ppu.Write( PPU_REG_DATA_SPRITES_ADDR, readData );
                intMemAddr++;
            }

        }
        else
        {
            m_Apu.Write( busAddr, busData );
            m_Control.Write( busAddr, busData );
        }

    }
    else if( busAddr >= CARTRIDGE_SWITCHED_ROM_BASE_CPU_ADDR )
    {
        m_pMapper->Write( busAddr, busData );
    }
    else// if( busAddr >= CARTRIDGE_RAM_BASE_CPU_ADDR )
    {
        busAddr -= CARTRIDGE_RAM_BASE_CPU_ADDR;
        m_CartridgeRam[ busAddr ] = busData;
    }
}

uint8_t GameConsole_t::CpuBusRead( uint16_t busAddr )
{

    uint8_t readData = 0;

    
    if( busAddr < 0x2000)
    {
        busAddr &= 0x07FF; // CPU RAM mirroring
        readData = m_Ram[ busAddr ];
    }
    else if( busAddr <= 0x2FFA )
    {

        readData = m_Ppu.Read(busAddr);
    }
    else if( busAddr <= 0x4017 )
    {
        readData = m_Apu.Read( busAddr );

        if( ( busAddr == 0x4016) || ( busAddr == 0x4017) ) 
        {
            readData = m_Control.Read( busAddr );
        }
    }
    else if( busAddr >= CARTRIDGE_SWITCHED_ROM_BASE_CPU_ADDR )
    {
        uint32_t offset = ROM_FILE_HEADER_SIZE + m_pMapper->GetRomAddrFromCpuAddr( busAddr );
        readData = m_pGameRomFile[ offset ];
    }
    else// if( busAddr >= CARTRIDGE_RAM_BASE_CPU_ADDR )
    {
        busAddr -= CARTRIDGE_RAM_BASE_CPU_ADDR;
        readData = m_CartridgeRam[ busAddr ];
    }

    return readData;
}

void GameConsole_t::PpuBusWrite( uint16_t busAddr, uint8_t busData )
{
    busAddr &= 0x3FFF;

    if( busAddr >= 0x2000 )
    {       
        uint16_t videoRamAddr = m_pMapper->TranslateVideoRamAddr( busAddr );    
                       
        m_CartrigeVideoRam[ videoRamAddr ] = busData;  
    }
    else
    {
        m_CartrigeChrRam[ busAddr ] = busData;
    }
    
}


uint8_t GameConsole_t::PpuBusRead( uint16_t busAddr )
{
    uint8_t data;

    //busAddr &= 0x3FFF;//!!!

    if( busAddr >= CARTRIDGE_PPU_VRAM_BASE_ADDR )
    {   
        uint16_t videoRamAddr = m_pMapper->TranslateVideoRamAddr( busAddr );               
        data = m_CartrigeVideoRam[ videoRamAddr ];
    }
    else if( 0 != m_GameRomInfo.NumberOfVideoRomBanks)
    {
        uint32_t offset = m_pMapper->GetRomAddrFromPpuAddr( busAddr );
        data = m_pGameRomFile[ offset ];
    }
    else
    {
        data = m_CartrigeChrRam[ busAddr ];
    }

    
    return data;
}
