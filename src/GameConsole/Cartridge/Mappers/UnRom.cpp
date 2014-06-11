/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "UnRom.h"

void UNROM_Mapper_t ::Init( void )
{
    m_RomAddrBank1 = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks - ROM_BANK_SIZE;
    m_RomAddrBank0 = m_RomAddrBank1 - ROM_BANK_SIZE;
}

void UNROM_Mapper_t::Write( uint16_t addr, uint8_t value )
{
    uint8_t nBank = 0x07 & value;
    m_RomAddrBank0 = nBank * ROM_BANK_SIZE;
}

uint32_t UNROM_Mapper_t::GetRomAddrFromCpuAddr( uint16_t cpuAddr )
{
    uint32_t retValue = 0;
        
    if(  MAPPER_UNROM_PROG_BANK1_ADDR <= cpuAddr )
    {
        retValue = m_RomAddrBank1 + cpuAddr - MAPPER_UNROM_PROG_BANK1_ADDR;
    }
    else if( MAPPER_UNROM_PROG_BANK0_ADDR <= cpuAddr)
    {
        retValue = m_RomAddrBank0 + cpuAddr - MAPPER_UNROM_PROG_BANK0_ADDR;
    }

    return retValue;
}