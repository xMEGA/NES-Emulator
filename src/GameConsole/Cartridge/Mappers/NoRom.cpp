/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "NoRom.h"

void NO_Mapper_t::Init( MapperInfo_t& mapperInfo )
{
    m_MapperInfo = mapperInfo;
    
    m_RomAddrBank0 = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks - 2 * ROM_BANK_SIZE;
    m_ChrAddrBank0 = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks;   
}

uint32_t NO_Mapper_t::GetRomAddrFromCpuAddr( uint16_t cpuAddr )
{
    uint32_t retValue;        
    retValue = m_RomAddrBank0 + cpuAddr - MAPPER_NO_PROG_BANK0_ADDR;

    return retValue;
}

uint32_t NO_Mapper_t::GetRomAddrFromPpuAddr( uint16_t ppuAddr )
{
    uint32_t retValue = 0;

    retValue = ROM_FILE_HEADER_SIZE + m_ChrAddrBank0 + ppuAddr;

    return retValue;
}