/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "IMapper.h"

#include "NoRom.h"
#include "UnRom.h"
#include "AoRom.h"
#include "Mmc3.h"
#include "Mmc1.h"


IMapper_t* IMapper_t::CreateMapper( MapperInfo_t* mapperInfo )
{
    IMapper_t* pIMapper = 0;

    switch( mapperInfo->CartridgeMapperType )
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

    if( pIMapper )
    {
        pIMapper->m_MapperInfo = *mapperInfo;
    }

    return pIMapper;
}


void IMapper_t::SetInterruptRequestCallBack( InterruptRequestCallBack_t pInterruptRequestCallBack, _in_ void * pContext )
{
    fp_IntRequestCallBack = pInterruptRequestCallBack;
    m_pContext = pContext;
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


void IMapper_t::Init( void )
{

}

IMapper_t::~IMapper_t()
{
}

void IMapper_t::Write( uint16_t addr, uint8_t value )
{

}

void IMapper_t::IrqCounterDecrement( void )
{

}

uint32_t IMapper_t::GetRomAddrFromCpuAddr( uint16_t cpuAddr )
{
    return 0; 
}

uint32_t IMapper_t::GetRomAddrFromPpuAddr( uint16_t ppuAddr )
{ 
    return 0;
}