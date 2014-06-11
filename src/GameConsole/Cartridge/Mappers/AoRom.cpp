/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "AoRom.h"


void AOROM_Mapper_t::Init( void )
{
    m_RomAddrBank0 = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks - MAPPER_AOROM_PROG_BANK0_ADDR;
    m_AddressPin10 = 0;
}


void AOROM_Mapper_t::Write( uint16_t addr, uint8_t value )
{
    uint8_t nBank = value & 0x07;
    
    m_RomAddrBank0 = MAPPER_AOROM_PROG_ROM_BANK_SIZE * nBank;
    
    if( !(value& 0x10) )
    {
        m_AddressPin10 = 1;      
    }
    else
    {
        m_AddressPin10 = 0; 
    }
}

uint32_t AOROM_Mapper_t::GetRomAddrFromCpuAddr( uint16_t cpuAddr )
{
    uint32_t retValue = 0;
    
    retValue = m_RomAddrBank0 + cpuAddr - MAPPER_AOROM_PROG_BANK0_ADDR;
    
    return retValue;
}

uint16_t AOROM_Mapper_t::TranslateVideoRamAddr( uint16_t addr )
{
    uint16_t videoRamAddr;    

    if( NO_MIRRORING_TYPE != m_MapperInfo.MirroringType )
    {            
        videoRamAddr =  ( addr & 0x03FF ) | (  m_AddressPin10 << 10 );
    }
    else
    {
        videoRamAddr = addr & 0x0FFF;
    }

    return videoRamAddr;
}