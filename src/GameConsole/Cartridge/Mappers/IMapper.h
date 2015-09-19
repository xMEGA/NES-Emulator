/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _IMapper_h_
#define _IMapper_h_

#include <stdint.h>
#include "CartridgeMapperTypes.h"
#include "../GameRomAnaliser.h"

#define SET_BIT 1
#define CLR_BIT 0

/*
CS is active
A10 = A11
Addr &= 0x07FF

0x2000
0x2000
0x2400
0x2400



CS is active
A10 = A10 
Addr &= 0x07FF

0x2000
0x2400
0x2000
0x2400

CS is not active
A10 = A10 
Addr &= 0x0FFF

0x2000
0x2400
0x2800
0x2C00


CS is active
A10 = 0 
Addr &= 0x07FF

0x2000
0x2000
0x2000
0x2000



CS is active
A10 = 1 
Addr &= 0x07FF

0x2400
0x2400
0x2400
0x2400

*/


/*
A10 0x2000 / 0x2400

A13

            VRAM (In Console)

PPU (PA0)  ->             VRAM (A0)
PPU (PA1)  ->           VRAM (A1) 
PPU (PA2)  ->           VRAM (A2)
PPU (PA3)  ->           VRAM (A3)
PPU (PA4)  ->           VRAM (A4)
PPU (PA5)  ->           VRAM (A5)
PPU (PA6)  ->           VRAM (A6)
PPU (PA7)  ->           VRAM (A7)
PPU (PA8)  ->           VRAM (A8)
PPU (PA9)  ->                VRAM (A9)
PPU (PA10) -> Cartridge -> VRAM (A10)
PPU (PA11) -> Cartridge ->
PPU (PA12) -> Cartridge ->
PPU (PA13) -> Cartridge -> VRAM (CS)


                                              A11 -> A10
                   Addr&=0x07FF;                  Addr &= 0x07FF            
                
    
0x2000    10000000000000      0x2000    10000000000000        0x2000        10000000000000

0x2400    10010000000000      0x2400    10010000000000        0x2000        10000000000000

0x2800    10100000000000      0x2000    10000000000000        0x2400        10010000000000

0x2C00    10110000000000      0x2400    10010000000000        0x2400        10010000000000
*/
    


//#define CARTRIDGE_SWITCHED_ROM_SIZE                 0x4000
//#define CARTRIDGE_CHR_RAM_BASE_CPU_ADDR             0x6000


#define CARTRIDGE_VIDEO_ROM_BASE_PPU_ADDR           0x0000
#define CARTRIDGE_VIDEO_ROM_SIZE                    0x2000
#define CARTRIDGE_NON_SWITCHED_ROM_BASE_CPU_ADDR    0xC000

#define CARTRIDGE_RAM_BASE_CPU_ADDR                 0x6000
#define CARTRIDGE_RAM_SIZE                          0x2000

#define CARTRIDGE_SWITCHED_ROM_BASE_CPU_ADDR        0x8000

#define CARTRIDGE_PPU_VRAM_BASE_ADDR                0x2000
#define CARTRIDGE_PPU_VRAM_SIZE                     0x1000

struct MapperInfo_t
{
    CartridgeMapperType_t   CartridgeMapperType;
    MirroringType_t         MirroringType;      //Vertical/horizontal mirroring
    uint8_t                 NumberOfRomBanks;   //Amount of 16kB ROM banks
};

typedef void (*RomFileAccesCallBack_t)( void * context, uint8_t* pData, uint32_t offset, uint16_t bytesCnt );
typedef void (*InterruptRequestCallBack_t)( void * pContext );


class IMapper_t
{
public:
    virtual void Init( MapperInfo_t& mapperInfo );
    virtual void SetInterruptRequestCallBack( InterruptRequestCallBack_t pInterruptRequestCallBack, void * pContext );
    virtual void Write( uint16_t addr, uint8_t value );
    virtual uint32_t GetRomAddrFromCpuAddr( uint16_t cpuAddr );
    virtual uint32_t GetRomAddrFromPpuAddr( uint16_t ppuAddr );
    virtual uint16_t TranslateVideoRamAddr( uint16_t addr );
    virtual void IrqCounterDecrement( void );
    virtual ~IMapper_t();
    
protected:
    MapperInfo_t                m_MapperInfo;
    void*                       m_pContext;
    InterruptRequestCallBack_t  fp_IntRequestCallBack;

private:
    RomFileAccesCallBack_t     RomFileAccesCallBack;
};

#endif
