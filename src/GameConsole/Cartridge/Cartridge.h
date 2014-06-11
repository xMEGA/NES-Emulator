/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _Cartridge_h_
#define _Cartridge_h_

#include <stdint.h>
#include "GameRomAnaliser.h"
#include "Mappers/IMapper.h"

#define _in_
#define _out_
#define _in_out_

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
    
#define CARTRIDGE_VIDEO_ROM_BASE_PPU_ADDR           0x0000
#define CARTRIDGE_VIDEO_ROM_SIZE                    0x2000
#define CARTRIDGE_NON_SWITCHED_ROM_BASE_CPU_ADDR    0xC000

#define CARTRIDGE_RAM_BASE_CPU_ADDR                 0x6000
#define CARTRIDGE_RAM_SIZE                          0x2000

#define CARTRIDGE_SWITCHED_ROM_BASE_CPU_ADDR        0x8000

//#define CARTRIDGE_SWITCHED_ROM_SIZE                 0x4000
//#define CARTRIDGE_CHR_RAM_BASE_CPU_ADDR             0x6000



class Cartridge_t
{

    public:
        void SetInterruptRequestCallBack( InterruptRequestCallBack_t pInterruptRequestCallBack, _in_ void * pContext ); 
        void SetRomFileAccesCallBack( RomFileAccesCallBack_t pRomFileAccesCallBack, _in_ void * pContext );
        void Reset(void);    
        void IrqCounterDecrement( void );
        
        Cartridge_t();

        uint8_t CpuRead( _in_ uint16_t addr );
        void CpuWrite( _in_ uint16_t addr, _in_ uint8_t data);

        uint8_t PpuRead( _in_ uint16_t addr );
        void PpuWrite( _in_ uint16_t addr, _in_ uint8_t data);    
         
    private:
        GameRomInfo_t               m_GameRomInfo;
        GameRomAnaliser_t           m_GameRomAnaliser;
            
        uint8_t                     m_CartrigeChrRam   [ 0x2000 ];
        uint8_t                     m_CartrigeVideoRam [ 0x1000 ];
        uint8_t                     m_CartridgeRam     [ 0x2000 ];
        
    private:
        IMapper_t*                  m_pMapper;
        void*                       m_pContext;
        RomFileAccesCallBack_t      fp_RomFileAccesCallBack;
        InterruptRequestCallBack_t  fp_InterruptRequestCallBack;
};

#endif