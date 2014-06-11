/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "GameRomAnaliser.h"

GameRomInfo_t GameRomAnaliser_t::GetGameRomInfo(uint8_t* pRom)
{
    GameRomInfo_t romInfo;
    uint8_t* pSource;
    uint8_t* pDestination;
    uint8_t  byte;
    uint8_t  mapperType;

    pSource = pRom;
    pDestination = romInfo.GameRomSignature;

    for(uint8_t i=0; i != ROM_SIGNATURE_SIZE; i++)
    {
        *pDestination ++= *pSource++;
    }

    romInfo.NumberOfRomBanks = *pSource++;
    romInfo.NumberOfVideoRomBanks = *pSource++;

    byte = *pSource++;
    
    ( VERTICAL_MIRRORING_BIT & byte ) ? romInfo.MirroringType = VERTICAL_MIRRORING_TYPE : romInfo.MirroringType = HORIZONTAL_MIRRORING_TYPE; 
    ( BATTERY_BACKET_RAM_BIT & byte ) ? romInfo.BatteryBackedRam = true : romInfo.BatteryBackedRam = false; 
    if( FOUR_SCREEN_LAYOUT_BIT & byte )  
        romInfo.MirroringType = NO_MIRRORING_TYPE;

    (TRAINER_BIT & byte)? romInfo.Trainer = true : romInfo.Trainer = false; 
    mapperType = MAPPER_LOW_PART_BITS & (byte >> 4);
    
    byte = *pSource++;

    (VS_SYSTEM_CERTRIGESS_BIT & byte) ? romInfo.VsSystemCartridgess = true : romInfo.VsSystemCartridgess = false;  
    mapperType |= MAPPER_HIGH_PART_BITS & (byte >> 4);

    romInfo.CartridgeMapperType = static_cast<CartridgeMapperType_t>(mapperType);

    romInfo.NumberOfRamBanks = *pSource++;
    byte = *pSource;

    if(byte & PAL_ENCODING_SYSTEM_BIT)
    {
        romInfo.ColorEncSystem = PAL_ENCODING_SYSTEM;
    }
    else
    {
        romInfo.ColorEncSystem = NTSC_ENCODING_SYSTEM;
    }

    return romInfo;
}