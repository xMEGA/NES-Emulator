#ifndef _GameRomAnaliser_h_
#define _GameRomAnaliser_h_

#include <stdint.h>
#include "Mappers/CartridgeMapperTypes.h"


#define ROM_SIGNATURE_SIZE   4
#define ROM_BANK_SIZE        0x4000
#define ROM_CHR_BANK_SIZE    0x2000
#define ROM_FILE_HEADER_SIZE 16


enum ColorEncodingSystem_t
{
    NTSC_ENCODING_SYSTEM = 0,
    PAL_ENCODING_SYSTEM  = 1,
};

enum MirroringType_t
{
    VERTICAL_MIRRORING_TYPE   = 0,
    HORIZONTAL_MIRRORING_TYPE = 1,
    NO_MIRRORING_TYPE         = 2
};


struct GameRomInfo_t
{
    bool                    Trainer;                                 //True for a 512-byte trainer at $7000-$71FF
    ColorEncodingSystem_t   ColorEncSystem;                          //PAL/NTSC cartridges 
    bool                    VsSystemCartridgess;                     //True for VS-System cartridgess
    //  bool                    FourScreenVideoRamLayout;              //True for a four-screen VRAM layout
    bool                    BatteryBackedRam;                        //True for battery-backed RAM at 0x6000-0x7FFF
    MirroringType_t         MirroringType;                           //Vertical/horizontal mirroring
    CartridgeMapperType_t   CartridgeMapperType;                     //ROM Mapper type
    uint8_t                 NumberOfVideoRomBanks;                   //Amount of 8kB VROM banks
    uint8_t                 NumberOfRomBanks;                        //Amount of 16kB ROM banks
    uint8_t                 NumberOfRamBanks;                        //Amount of 8kB RAM banks. For compatibility with the previous versions of the .NES format, assume 1x8kB RAM page when this byte is zero
    uint8_t                 GameRomSignature[ROM_SIGNATURE_SIZE];    //String "NES^Z" used to recognize .NES files
};

class GameRomAnaliser_t
{
    public:
        GameRomInfo_t GetGameRomInfo(uint8_t* pRom);
           
    private:
        enum //RomInfoBits_t
        {
            VERTICAL_MIRRORING_BIT     = 0x01,
            BATTERY_BACKET_RAM_BIT     = 0x02,
            TRAINER_BIT                = 0x04,
            FOUR_SCREEN_LAYOUT_BIT     = 0x08,    
            MAPPER_LOW_PART_BITS       = 0x0F
        };
        
        enum
        {
            VS_SYSTEM_CERTRIGESS_BIT   = 0x01,
            MAPPER_HIGH_PART_BITS      = 0xF0
        };

        enum 
        {
            PAL_ENCODING_SYSTEM_BIT    = 0x01
        };
};

#endif