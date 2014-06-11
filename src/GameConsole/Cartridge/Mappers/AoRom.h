/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _AoRom_h_
#define _AoRom_h_

#include <stdint.h>
#include "IMapper.h"

#define _in_
#define _out_
#define _in_out_

#define MAPPER_AOROM_PROG_ROM_BANK_SIZE 0x8000
#define MAPPER_AOROM_PROG_BANK0_ADDR    0x8000


/*
 _______  0x8000 - switching ( BANK0 )
|       |
|       |
|       | 
|       |
|       |
|_______| 0xFFFF

*/

class AOROM_Mapper_t : public IMapper_t 
{
public:
    void Init( void );
    void Write( uint16_t addr, uint8_t value );
    uint32_t GetRomAddrFromCpuAddr( uint16_t cpuAddr );
    uint16_t TranslateVideoRamAddr( uint16_t addr );

private:
    uint32_t m_RomAddrBank0;
    uint16_t m_AddressPin10;
};

#endif