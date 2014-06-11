/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _UnRom_h_
#define _UnRom_h_

#include <stdint.h>
#include "IMapper.h"

#define _in_
#define _out_
#define _in_out_

/*
 _______  0x8000 - switching ( BANK0 )
|       |
|       |
|_______| 0xC000 - non switching ( BANK1 )
|       |
|       |
|_______| 0xFFFF

*/

#define MAPPER_UNROM_PROG_BANK0_ADDR 0x8000
#define MAPPER_UNROM_PROG_BANK1_ADDR 0xC000


class UNROM_Mapper_t : public IMapper_t 
{
    public:
        void Init( void );
        void Write( uint16_t addr, uint8_t value );
        uint32_t GetRomAddrFromCpuAddr( uint16_t cpuAddr );

    private:
        uint32_t m_RomAddrBank0;
        uint32_t m_RomAddrBank1;
};

#endif