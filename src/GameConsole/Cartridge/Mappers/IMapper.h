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
    IMapper_t* CreateMapper( MapperInfo_t* mapperInfo );
    virtual void SetInterruptRequestCallBack( InterruptRequestCallBack_t pInterruptRequestCallBack, void * pContext );
    virtual void Init( void );
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