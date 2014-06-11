/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Mmc1.h"

void MMC1_Mapper_t::Init( void )
{
    m_RomAddrBank[1] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks - MAPPER_MMC1_PROG_ROM_BANK_SIZE;
    m_RomAddrBank[0] = m_RomAddrBank[1] - MAPPER_MMC1_PROG_ROM_BANK_SIZE;
        
    m_ChrAddrBank[1] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks;
    m_ChrAddrBank[0] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks + MAPPER_MMC1_CHR_ROM_BANK_SIZE;

    m_BitPos        = 0;
    m_ShiftRegValue = 0; 
}

void MMC1_Mapper_t::Write( uint16_t addr, uint8_t value )
{

    m_LoadReg.Value = value;

    if( m_LoadReg.ResetShiftRegister )
    {
        m_ControlReg.ProgRomBankMode = 0x03;
        m_BitPos        = 0;
        m_ShiftRegValue = 0;
    }    
    else    
    {
        m_ShiftRegValue |= ( m_LoadReg.DataShift << m_BitPos );      
        m_BitPos ++;

        if( NUMBER_OF_CLOCK_FOR_FULL_LATCH == m_BitPos )
        {
             addr &= 0xE000; // !!! Проверить

             switch( addr )
             {
                 case MMC1_CONTROL_REG_ADDR:
                    m_ControlReg.Value = m_ShiftRegValue;

                 break;
             
                 case MMC1_CHR_BANK0_REG_ADDR:
                    m_ChrBank0Reg = m_ShiftRegValue & 0x1F;

                    if( m_ControlReg.ChrRomBankMode ) 
                    {
                        m_ChrAddrBank[0] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks + m_ChrBank0Reg * MAPPER_MMC1_CHR_ROM_BANK_SIZE;          
                    } 
                    else // !!! Проверить
                    {
                        m_ChrAddrBank[0] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks + MAPPER_MMC1_CHR_ROM_BANK_SIZE * ( m_ChrBank0Reg / 2 );
                        m_ChrAddrBank[1] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks + MAPPER_MMC1_CHR_ROM_BANK_SIZE * m_ChrBank0Reg;
                    }
                 break;
             
                 case MMC1_CHR_BANK1_REG_ADDR:
                    m_ChrBank1Reg = m_ShiftRegValue & 0x1F;

                    if( m_ControlReg.ChrRomBankMode )
                    {
                        m_ChrAddrBank[1] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks + m_ChrBank1Reg * MAPPER_MMC1_CHR_ROM_BANK_SIZE;
                    }
                        
                 break;
             
                 case MMC1_PROG_BANK0_REG_ADDR:
                 
                 m_ProgBankReg.Value = m_ShiftRegValue;
                 
                 switch( m_ControlReg.ProgRomBankMode )
                 {
                    case 0: // !!! Проверить
                        m_RomAddrBank[0] = ROM_BANK_SIZE * ( m_ProgBankReg.NumberOfBank / 2 );
                        m_RomAddrBank[1] = ROM_BANK_SIZE * m_ProgBankReg.NumberOfBank;
                    break;

                    case 2: // !!! Проверить
                        m_RomAddrBank[0] = 0; 
                        m_RomAddrBank[1] = ROM_BANK_SIZE * m_ProgBankReg.NumberOfBank;
                    break;

                    case 3:
                        m_RomAddrBank[1] = ROM_BANK_SIZE * m_MapperInfo.NumberOfRomBanks -  MAPPER_MMC1_PROG_ROM_BANK_SIZE;
                        m_RomAddrBank[0] = ROM_BANK_SIZE * m_ProgBankReg.NumberOfBank;        
                    break;

                    default:
                    break;
                 }

                 break;
             }
             
             m_BitPos        = 0;
             m_ShiftRegValue = 0; 
        }
    }

}

uint32_t MMC1_Mapper_t::GetRomAddrFromCpuAddr( uint16_t cpuAddr )
{
    uint32_t retValue = 0;
    uint16_t addrInBank = cpuAddr & 0x3FFF;

    uint8_t nBank = cpuAddr >> 14;
    nBank &= 0x01;

    retValue = m_RomAddrBank[ nBank ] + addrInBank;

    return retValue;
}

uint32_t MMC1_Mapper_t::GetRomAddrFromPpuAddr( uint16_t ppuAddr )
{
    uint32_t retValue = 0;
    uint8_t nBank = ppuAddr >> 12;
    uint16_t addrInBank = ppuAddr & 0x0FFF;

    nBank &= 0x01;

    retValue = m_ChrAddrBank[nBank] + addrInBank;

    return retValue;
}


uint16_t MMC1_Mapper_t::IsSetAddressPin10( uint16_t addr )
{
    uint16_t A10 = 0;
    
    switch( m_ControlReg.Mirroring )
    {
        case 0:  // !!!
            A10 = CLR_BIT;
        break;

        case 1: // !!!
            A10 = SET_BIT;
        break;

        case 2: 
            addr >>= 10;
            addr &= 0x0001;
            A10 = addr;
        break;

        case 3: 
            addr >>= 11;
            addr &= 0x0001;
            A10 = addr; //!!!
        break;

        default:
        break;
    }
    return A10;
}

uint16_t MMC1_Mapper_t::TranslateVideoRamAddr( uint16_t addr )
{
    uint16_t videoRamAddr;    

    if( NO_MIRRORING_TYPE != m_MapperInfo.MirroringType )
    {            
        videoRamAddr =  ( addr & 0x03FF ) | (  IsSetAddressPin10( addr ) << 10 );
    }
    else
    {
        videoRamAddr = addr & 0x0FFF;
    }

    return videoRamAddr;
}