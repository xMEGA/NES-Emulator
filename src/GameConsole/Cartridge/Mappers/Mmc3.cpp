/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Mmc3.h"


void MMC3_Mapper_t::Init( void )
{
    for( uint8_t i = 0; i != 4; i++ )
    {
        m_RomAddrBank[i] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks - (4 - i) * MAPPER_MMC3_PROG_ROM_BANK_SIZE;
    }

    for( uint8_t i = 0; i != 8; i++ )
    {
        m_ChrAddrBank[i] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks + i * MAPPER_MMC3_CHR_ROM_BANK_SIZE;
    }
      
    m_IsIrqEnable   = true;
    m_IsCopyByNextClk = false;
    m_IrqCounter    = 0xFF;
    m_IrqLatchValue = 0xFF;
}

void MMC3_Mapper_t::Write( uint16_t addr, uint8_t value )
{
    uint8_t nBank = 0;
    addr &= 0xE001; // !!!
       
    switch( addr )
    {
        case MMC3_CONTROL_REG_ADDR:
            m_ControlReg.Value = value;
        break;

        case MMC3_BANK_DATA_REG_ADDR:
            m_DataRegister = value;
            switch( m_ControlReg.Mode )
            {
            case 0:
            case 1:
                m_ChrAddrBank[ 2 * m_ControlReg.Mode + 0 + 4 * m_ControlReg.ChrAddrInvert ] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks + 0x400 * ( value + 0 );
                m_ChrAddrBank[ 2 * m_ControlReg.Mode + 1 + 4 * m_ControlReg.ChrAddrInvert ] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks + 0x400 * ( value + 1 );                     
            break;

            case 2:
            case 3:
            case 4:
            case 5:
                   m_ChrAddrBank[ m_ControlReg.Mode + 2 - 4 * m_ControlReg.ChrAddrInvert ] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks + 0x400 * value;                
            break;

            case 6:
                if( m_ControlReg.SwapControl )
                {
                    nBank = value & ( 2 * m_MapperInfo.NumberOfRomBanks - 1 );

                    m_RomAddrBank[0] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks - 2 * MAPPER_MMC3_PROG_ROM_BANK_SIZE;             
                    m_RomAddrBank[2] = MAPPER_MMC3_PROG_ROM_BANK_SIZE * nBank; 
                    
                }
                else
                {
                    nBank = value & ( 2 * m_MapperInfo.NumberOfRomBanks - 1 );

                    m_RomAddrBank[2] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks - 2 * MAPPER_MMC3_PROG_ROM_BANK_SIZE;             
                    m_RomAddrBank[0] = MAPPER_MMC3_PROG_ROM_BANK_SIZE * nBank;                             
                
                }
            break;

            case 7:
                 nBank = value & ( 2 * m_MapperInfo.NumberOfRomBanks - 1 );
                 m_RomAddrBank[1] = MAPPER_MMC3_PROG_ROM_BANK_SIZE * nBank; 

            break;

            }
        break;

        case MMC3_MIRRORING_REG_ADDR:
            m_MirroringReg.Value = value;
            m_MapperInfo.MirroringType = static_cast<MirroringType_t>(m_MirroringReg.MirroringType);
        break;

        case MMC3_WRAM_PROTECT_REG_ADDR:
            m_WramProtectReg.Value = value;
        break;

        case MMC3_IRQ_LATCH_REG_ADDR:
            m_IrqLatchValue  = value;
        break;

        case MMC3_IRQ_RELOAD_REG_ADDR:
            m_IrqCounter = 0;
            m_IsCopyByNextClk = true;
            //m_IrqCounter = m_IrqLatchValue;
        break;

        case MMC3_IRQ_DISABLE_REG_ADDR:
            m_IsIrqEnable = false;
        break;

        case MMC3_IRQ_ENABLE_REG_ADDR:
            m_IsIrqEnable = true;
        break;
     }

}

uint32_t MMC3_Mapper_t::GetRomAddrFromCpuAddr( uint16_t cpuAddr )
{
    uint32_t retValue = 0;
    uint16_t addrInBank = cpuAddr & 0x1FFF;

    uint8_t nBank = cpuAddr >> 13;
    nBank &= 0x03;

    retValue = m_RomAddrBank[ nBank ] + addrInBank;

    return retValue;
}

uint32_t MMC3_Mapper_t::GetRomAddrFromPpuAddr( uint16_t ppuAddr )
{
    uint32_t retValue = 0;
    uint8_t nBank = ppuAddr>>10;
    
    nBank &= 0x07;

    retValue = m_ChrAddrBank[nBank] + ( ppuAddr&0x3FF );

    return retValue;
}

void MMC3_Mapper_t::IrqCounterDecrement( void )
{
    if( true == m_IsCopyByNextClk )
    {
        m_IsCopyByNextClk = false;
        m_IrqCounter = m_IrqLatchValue;
    }

    if ( m_IrqCounter > 0 ) 
    {
        m_IrqCounter --;
    } 
    else 
    {
        if( m_IsIrqEnable /*&& m_IrqLatchValue*/ )
        {
            fp_IntRequestCallBack( m_pContext );
        }
        m_IrqCounter = m_IrqLatchValue;
    }
}