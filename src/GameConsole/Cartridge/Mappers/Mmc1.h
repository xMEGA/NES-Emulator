/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _Mmc1_h_
#define _Mmc1_h_

#include <stdint.h>
#include "IMapper.h"

#define _in_
#define _out_
#define _in_out_


// ----------------------- Mapper Registers ------------------------------ 

#define MMC1_CONTROL_REG_ADDR       0x8000
#define MMC1_CHR_BANK0_REG_ADDR     0xA000
#define MMC1_CHR_BANK1_REG_ADDR     0xC000
#define MMC1_PROG_BANK0_REG_ADDR    0xE000

union MMC1_LoadReg_t               // 0x8000 - 0xFFFF
{
    struct
    {
        uint8_t DataShift          : 1; // Data bit to be shifted into shift register, LSB first
        uint8_t                    : 6;
        uint8_t ResetShiftRegister : 1; // Reset shift register and write Control with (Control OR $0C),locking PRG ROM at $C000-$FFFF to the last bank.
    };
    uint8_t Value;
};

union MMC1_ControlReg_t            // 0x8000 - 0x9FFF
{
    struct
    {
        uint8_t Mirroring          : 2; //     0: one-screen, lower bank
                                        //     1: one-screen, upper bank 
                                        //     2: vertical 
                                        //     3: horizontal

        uint8_t ProgRomBankMode    : 2; //  0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
                                        //     2: fix first bank at $8000 and switch 16 KB bank at $C000;
                                        //     3: fix last bank at $C000 and switch 16 KB bank at $8000

        uint8_t ChrRomBankMode     : 1; //     0: switch 8 KB at a time; 
                                        //     1: switch two separate 4 KB banks
    };
    uint8_t Value;
};

union MMC1_ProgBankReg_t           // 0xE000 - 0xFFFF
{
    struct
    {
        uint8_t NumberOfBank       : 4; // Select 16 KB PRG ROM bank (low bit ignored in 32 KB mode)
        uint8_t ProgRamChipEnable  : 1; // PRG RAM chip enable (ignored on MMC1A)
                                        // 0: enabled; 
                                        // 1: disabled;
    };
    uint8_t Value;
};
// ----------------------------------------------------------------------- 


// ---------------------------- PROG Banks -------------------------------

#define MAPPER_MMC1_PROG_RAM_BANK_SIZE          0x2000
#define MAPPER_MMC1_PROG_RAM_BANK0_BASE_ADDR    0x6000

#define MAPPER_MMC1_PROG_ROM_BANK_SIZE          0x4000
#define MAPPER_MMC1_PROG_ROM_BANK0_BASE_ADDR    0x8000
#define MAPPER_MMC1_PROG_ROM_BANK1_BASE_ADDR    0xC000

/*
 _______  0x6000                                  ( PROG_RAM_BANK0 )
|       |
|       |
|  RAM  | 
|       |
|_______| 0x8000 - переключаемый банк картриджа   ( PROG_ROM_BANK0 )
|       |
|       |
|  ROM  | 
|       |
|_______| 0xC000 - переключаемый банк картриджа   ( PROG_ROM_BANK1 )
|       |
|       |
|  ROM  | 
|       |
|_______| 0xFFFF

*/

// -----------------------------------------------------------------------


// ---------------------------- CHR Banks --------------------------------
#define MAPPER_MMC1_CHR_ROM_BANK_SIZE          0x1000

#define MAPPER_MMC1_CHR_ROM_BANK0_BASE_ADDR    0x0000  
#define MAPPER_MMC1_CHR_ROM_BANK1_BASE_ADDR    0x1000  

/*
 _______  0x0000 - переключаемый CHR банк картриджа   ( CHR_ROM_BANK0 )
|       |
|       |
|  ROM  | 
|       |
|_______| 0x1000 - переключаемый CHR банк картриджа   ( CHR_ROM_BANK1 )
|       |
|       |
|  ROM  | 
|       |
|_______| 0x2000

*/

// -----------------------------------------------------------------------

#define NUMBER_OF_CLOCK_FOR_FULL_LATCH    5

class MMC1_Mapper_t : public IMapper_t
{
    public:
        void Init( void );
        void Write( uint16_t addr, uint8_t value );
        
        uint32_t GetRomAddrFromCpuAddr( uint16_t cpuAddr );
        uint32_t GetRomAddrFromPpuAddr( uint16_t ppuAddr );
		
		uint16_t TranslateVideoRamAddr( uint16_t addr );

	private:
		uint16_t IsSetAddressPin10( uint16_t addr );

    private:
        uint32_t m_RomAddrBank[2];
        uint32_t m_ChrAddrBank[8];

    private: // Registers
        MMC1_LoadReg_t     m_LoadReg;
        MMC1_ControlReg_t  m_ControlReg;
        uint8_t            m_ChrBank0Reg; // [4..0] Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode)
        uint8_t            m_ChrBank1Reg; // [4..0] Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)
        MMC1_ProgBankReg_t m_ProgBankReg;

    private:      
        uint8_t m_BitPos;
        uint8_t m_ShiftRegValue;
 };

#endif