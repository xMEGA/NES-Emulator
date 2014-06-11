/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Cartridge.h"
#include "GameRomAnaliser.h"

Cartridge_t::Cartridge_t()
{
    m_pMapper = 0;
}

void Cartridge_t::SetInterruptRequestCallBack( InterruptRequestCallBack_t pInterruptRequestCallBack, _in_ void * pContext )
{
    fp_InterruptRequestCallBack = pInterruptRequestCallBack;
    m_pContext = pContext;
}

void Cartridge_t::SetRomFileAccesCallBack( RomFileAccesCallBack_t pRomFileAccesCallBack, _in_ void * pContext )
{
    fp_RomFileAccesCallBack = pRomFileAccesCallBack;
    m_pContext = pContext;
}

void Cartridge_t::Reset(void)
{
    uint8_t gameRomInfoBuffer[ ROM_FILE_HEADER_SIZE ];
    MapperInfo_t mapperInfo;

    fp_RomFileAccesCallBack( m_pContext, gameRomInfoBuffer, 0, ROM_FILE_HEADER_SIZE );
    m_GameRomInfo = m_GameRomAnaliser.GetGameRomInfo( gameRomInfoBuffer );
    
    mapperInfo.CartridgeMapperType = m_GameRomInfo.CartridgeMapperType;
    mapperInfo.MirroringType       = m_GameRomInfo.MirroringType;
    mapperInfo.NumberOfRomBanks    = m_GameRomInfo.NumberOfRomBanks;

    if( m_pMapper )
    {
        delete m_pMapper;
    }

    m_pMapper = new IMapper_t;
  
    m_pMapper = m_pMapper->CreateMapper( &mapperInfo );
       
    if( m_pMapper )
    {
    m_pMapper->SetInterruptRequestCallBack( fp_InterruptRequestCallBack, m_pContext );
        m_pMapper->Init();
    }
}

uint8_t Cartridge_t::CpuRead( _in_ uint16_t addr )
{
    uint8_t retValue = 0;// = 0; /// !!! JIM II 0x50xx addr
    uint32_t offset;
        
    if( addr >= CARTRIDGE_SWITCHED_ROM_BASE_CPU_ADDR )
    {
        offset = ROM_FILE_HEADER_SIZE + m_pMapper->GetRomAddrFromCpuAddr( addr );
        fp_RomFileAccesCallBack( m_pContext, &retValue, offset , sizeof( uint8_t ) ); // Подгружаем
    }
    else if( addr >= CARTRIDGE_RAM_BASE_CPU_ADDR )
    {
        addr -= CARTRIDGE_RAM_BASE_CPU_ADDR;
        retValue = m_CartridgeRam[ addr ];
    }
    

    return retValue;
}

void Cartridge_t::CpuWrite( _in_ uint16_t addr, _in_ uint8_t data )
{
    if( addr >= CARTRIDGE_SWITCHED_ROM_BASE_CPU_ADDR )
    {
        m_pMapper->Write( addr, data );
    }
    else if( addr >= CARTRIDGE_RAM_BASE_CPU_ADDR )
    {
        addr -= CARTRIDGE_RAM_BASE_CPU_ADDR;
        m_CartridgeRam[ addr ] = data;
    }
}


uint8_t Cartridge_t::PpuRead( _in_ uint16_t addr )
{
    uint8_t  retValue;
    uint16_t videoRamAddr;
    uint32_t offset;
       
    if( addr >= 0x2000 )
    {   
        videoRamAddr = m_pMapper->TranslateVideoRamAddr( addr );               
        retValue = m_CartrigeVideoRam[ videoRamAddr ];
    }
    else
    {
        if( 0 != m_GameRomInfo.NumberOfVideoRomBanks)
        {
            offset = ROM_FILE_HEADER_SIZE + m_pMapper->GetRomAddrFromPpuAddr( addr );
            fp_RomFileAccesCallBack( m_pContext, &retValue, offset , 1 );
        }
        else
        {
            retValue =    m_CartrigeChrRam[addr]; // !!!
        }  
    }

    return retValue;
}

void Cartridge_t::PpuWrite( _in_ uint16_t addr, _in_ uint8_t data )
{
    uint16_t videoRamAddr;
    
    if( addr >= 0x2000 )
    {       
        videoRamAddr = m_pMapper->TranslateVideoRamAddr( addr );    
                       
        m_CartrigeVideoRam[ videoRamAddr ] = data;  
    }
    else
    {
        m_CartrigeChrRam[addr] = data;
    }
 }

 void Cartridge_t::IrqCounterDecrement( void )
 {
    m_pMapper->IrqCounterDecrement();
 }