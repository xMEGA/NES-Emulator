/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _Mmc3_h_
#define _Mmc3_h_

#include <stdint.h>
#include "IMapper.h"

#define _in_
#define _out_
#define _in_out_

// ------------------ MMC3 Mapper -----------------------//

/*

10 - CIRAM A10(n)  

14 - PRG /CE (n)   
17 - PRG R/W (n)       

24 - PRG A13 (n)
20 - PRG A14 (n)    


34 - PRG D0 (s)
36 - PRG D1 (s)
38 - PRG D2 (s)
39 - PRG D3 (s)
37 - PRG D4 (s)
35 - PRG D5 (s)
33 - PRG D6 (s)
31 - PRG D7 (s)



32 - PRG A0 (s)

29 - M2
13 - /IRQ          


30 - WRAM /CE (w)
41 - WRAM +CE (w)
42 - WRAM /WE (w)



5  - CHR A10 (n)   
4  - CHR A11 (n)   
3  - CHR A12 (n)   


2  - CHR A10 (r)   
43 - CHR A11 (r)
9  - CHR A12 (r)   
7  - CHR A13 (r)   
8  - CHR A14 (r)   
11 - CHR A15 (r)   
44 - CHR A16 (r)
12 - CHR A17 (r)   


26 - PRG /CE (r)

19 - PRG A13 (r)   
22 - PRG A14 (r)   
18 - PRG A15 (r)   
21 - PRG A16 (r)   
25 - PRG A17 (r)
23 - PRG A18 (r)


1  - NC  
16 - NC  
27 - +5V
15 - GND  
40 - GND
6  - GND           
28 - GND

*/
// ------------------------------------------- //

//A15 A14 A13 A0

#define MMC3_CONTROL_REG_ADDR        0x8000
#define MMC3_BANK_DATA_REG_ADDR      0x8001

#define MMC3_MIRRORING_REG_ADDR      0xA000
#define MMC3_WRAM_PROTECT_REG_ADDR   0xA001


#define MMC3_IRQ_LATCH_REG_ADDR      0xC000
#define MMC3_IRQ_RELOAD_REG_ADDR     0xC001

#define MMC3_IRQ_DISABLE_REG_ADDR    0xE000
#define MMC3_IRQ_ENABLE_REG_ADDR     0xE001



union MMC3_ControlReg_t           // 0x8000 - 0x9FFE
{
    struct
    {
        uint8_t Mode          : 3;   
        uint8_t               : 3;
        uint8_t SwapControl   : 1;
        uint8_t ChrAddrInvert : 1;
    };
    uint8_t Value;
};



union MMC3_MirroringReg_t       // 0xA000
{
    struct
    {
        uint8_t MirroringType : 1;   // 0 - Vertical mirroring, 1 - Horizontal mirroring
        uint8_t               : 7;
    };
    uint8_t Value;
};


union MMC3_WramProtectReg_t    // 0xA001
{
    struct
    {
        uint8_t                  : 6;
        uint8_t WramWriteProtect : 1; // 0 - WRAM is writable, 1 - WRAM is readable, but writes do not proceed 
        uint8_t WramDisable      : 1; // 0 - Disable WRAM ( unmap WRAM totaly... reading from WRAM area results in open bus ), 1 - Enable WRAM... if present, it is mapped in 0x6000 - 0x7FFF
    };
    uint8_t Value;
};             



/*
 _______  0x8000 - переключаемый банк картриджа   ( PROG_BANK0 )
|       |
|       |
|_______| 0xA000 - переключаемый банк картриджа   ( PROG_BANK1 )
|       |
|       |
|_______| 0xC000 - переключаемый банк картриджа   ( PROG_BANK2 )
|       |
|       |
|_______| 0xE000 - непереключаемый банк картриджа ( PROG_BANK3 )
|       |
|       |
|_______| 0xFFFF
*/

#define MAPPER_MMC3_PROG_ROM_BANK_SIZE      0x2000
    
#define MAPPER_MMC3_PROG_BANK0_BASE_ADDR    0x8000
#define MAPPER_MMC3_PROG_BANK1_BASE_ADDR    0xA000
#define MAPPER_MMC3_PROG_BANK2_BASE_ADDR    0xC000
#define MAPPER_MMC3_PROG_BANK3_BASE_ADDR    0xE000


#define MAPPER_MMC3_CHR_ROM_BANK_SIZE       0x0400

#define MAPPER_MMC3_CHR_BANK0_BASE_ADDR     0x0000  // 1 KB switchable CHR bank
#define MAPPER_MMC3_CHR_BANK1_BASE_ADDR     0x0400  // 1 KB switchable CHR bank
#define MAPPER_MMC3_CHR_BANK2_BASE_ADDR     0x0800  // 1 KB switchable CHR bank
#define MAPPER_MMC3_CHR_BANK3_BASE_ADDR     0x0C00  // 1 KB switchable CHR bank
#define MAPPER_MMC3_CHR_BANK4_BASE_ADDR     0x1000  // 1 KB switchable CHR bank
#define MAPPER_MMC3_CHR_BANK5_BASE_ADDR     0x1400  // 1 KB switchable CHR bank
#define MAPPER_MMC3_CHR_BANK6_BASE_ADDR     0x1800  // 1 KB switchable CHR bank
#define MAPPER_MMC3_CHR_BANK7_BASE_ADDR     0x1C00  // 1 KB switchable CHR bank


/*
PPU $0000-$07FF (or $1000-$17FF): 2 KB switchable CHR bank
PPU $0800-$0FFF (or $1800-$1FFF): 2 KB switchable CHR bank
PPU $1000-$13FF (or $0000-$03FF): 1 KB switchable CHR bank
PPU $1400-$17FF (or $0400-$07FF): 1 KB switchable CHR bank
PPU $1800-$1BFF (or $0800-$0BFF): 1 KB switchable CHR bank
PPU $1C00-$1FFF (or $0C00-$0FFF): 1 KB switchable CHR bank
*/

class MMC3_Mapper_t: public IMapper_t
{
    public:
        void Init( void );
        void Write( uint16_t addr, uint8_t value ); 
        uint32_t GetRomAddrFromCpuAddr( uint16_t cpuAddr );
        uint32_t GetRomAddrFromPpuAddr( uint16_t ppuAddr );
        void IrqCounterDecrement( void );
 
   private:
        MMC3_ControlReg_t           m_ControlReg;
        uint8_t                     m_DataRegister;
        MMC3_MirroringReg_t         m_MirroringReg;
        MMC3_WramProtectReg_t       m_WramProtectReg;
        uint8_t                     m_IrqLatchValue;

    private:
        uint32_t                    m_RomAddrBank[4];
        uint32_t                    m_ChrAddrBank[8]; 
        uint16_t                    m_IrqCounter;
        bool                        m_IsIrqEnable;
        bool                        m_IsCopyByNextClk;
 };

#endif