/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _CartridgeMapperTypes_h_
#define _CartridgeMapperTypes_h_

enum CartridgeMapperType_t
{
    NO_MAPPER_MAPPER_TYPE     = 0,  //All 32kB ROM + 8kB VROM games
    NINTENDO_MMC1_MAPPER_TYPE = 1,  //Megaman2, Bomberman2, etc.
    UNROM_MAPPER_TYPE         = 2,  // MegaMan, Prince of Persia, Contra, Little Mermaid, Duck Tales 1,2      
    NINTENDO_MMC3_MAPPER_TYPE = 4,
    AOROM_MAPPER_TYPE         = 7,

    //Castlevania, LifeForce, etc.
    
    /*
    UNROM switch          //QBert, PipeDream, Cybernoid, many Japanese games
    Nintendo MMC3         //SilverSurfer, SuperContra, Immortal, etc.
    Nintendo MMC5         //Castlevania3
    FFE F4xxx             //F4xxx games off FFE CDROM
              //WizardsAndWarriors, Solstice, etc.
    FFE F3xxx             //F3xxx games off FFE CDROM
    Nintendo MMC2         //Punchout
    Nintendo MMC4         //Punchout2
    ColorDreams chip      //CrystalMines, TaginDragon, etc.
    FFE F6xxx             //F6xxx games off FFE CDROM
    CPROM switch
    100-in-1 switch       //100-in-1 cartridge
    Bandai chip           //Japanese DragonBallZ series, etc.
    FFE F8xxx             //F8xxx games off FFE CDROM
    Jaleco SS8806 chip    //Japanese Baseball3, etc.
    Namcot 106 chip       //Japanese GhostHouse2, Baseball90, etc.
    Nintendo DiskSystem   //Reserved. Don't use this mapper!
    Konami VRC4a          //Japanese WaiWaiWorld2, etc.
    Konami VRC2a          //Japanese TwinBee3
    Konami VRC2a          //Japanese WaiWaiWorld, MoonWindLegend, etc.
    Konami VRC6           
    Konami VRC4b          
    Irem G-101 chip       //Japanese ImageFight, etc.
    Taito TC0190/TC0350   //Japanese PowerBlazer
    Nina-1 board          //ImpossibleMission2 and DeadlyTowers
    Tengen RAMBO-1 chip
    Irem H-3001 chip
    GNROM switch
    SunSoft3 chip
    SunSoft4 chip
    SunSoft5 FME-7 chip
    Camerica chip
    Irem 74HC161/32-based
    AVE Nina-3 board      KrazyKreatures, DoubleStrike, etc.
    AVE Nina-6 board      Deathbots, MermaidsOfAtlantis, etc.
    Pirate HK-SF3 chip
    */    
};




#endif