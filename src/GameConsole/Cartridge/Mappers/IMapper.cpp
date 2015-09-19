/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "IMapper.h"


void IMapper_t::SetInterruptRequestCallBack( InterruptRequestCallBack_t pInterruptRequestCallBack, void * pContext )
{
    fp_IntRequestCallBack = pInterruptRequestCallBack;
    m_pContext = pContext;
}

void IMapper_t::Init( MapperInfo_t& mapperInfo )
{
    m_MapperInfo = mapperInfo;
}

uint16_t IMapper_t::TranslateVideoRamAddr( uint16_t addr )
{
    uint16_t videoRamAddr;    
    uint16_t pinA10;
    
    if( NO_MIRRORING_TYPE != m_MapperInfo.MirroringType )
    {            
        pinA10 = addr;

        pinA10 >>= static_cast<uint8_t>( m_MapperInfo.MirroringType );
        pinA10 >>= 10;
        pinA10 &= 0x0001;

        videoRamAddr =  ( addr & 0x03FF ) | (  pinA10 << 10 );
    }
    else
    {
        videoRamAddr = addr & 0x0FFF;
    }

    return videoRamAddr;
}

IMapper_t::~IMapper_t()
{
}

void IMapper_t::Write( uint16_t addr, uint8_t value )
{
    UNUSED( addr );
    UNUSED( value );


}

void IMapper_t::IrqCounterDecrement( void )
{

}

uint32_t IMapper_t::GetRomAddrFromCpuAddr( uint16_t cpuAddr )
{
    UNUSED( cpuAddr );
    return 0;
}

uint32_t IMapper_t::GetRomAddrFromPpuAddr( uint16_t ppuAddr )
{
    UNUSED( ppuAddr );
    return 0;
}
