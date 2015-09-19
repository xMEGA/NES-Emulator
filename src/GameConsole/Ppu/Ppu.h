/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _PpuCore_h_
#define _PpuCore_h_

#include <stdint.h>
#include "PpuRegisters.h"
#include "PpuPalette.h"


#define SET_BIT        1
#define CLR_BIT        0

#define PPU_SPRITES_PALETTE_SIZE            16
#define PPU_BG_PALETTE_SIZE                 16

#define PPU_PALETTE_BG_BASE_ADDR            0x3F00
#define PPU_PALETTE_SPRITES_BASE_ADDR       0x3F10

//#define PPU_CHR0_ROM_BASE_ADDR              0x0000
#define PPU_CHR1_ROM_BASE_ADDR              0x1000

#define PPU_HORIZONTAL_RESOLUTION           256
#define PPU_VERTICAL_RESOLUTION             240


#define PPU_CHR_SYMBOL_HEIGHT               8    //In pixels
#define PPU_CHR_SYMBOL_WIDTH                8    //In pixels
#define PPU_CHR_SYMBOL_SIZE                 16   //In bytes


#define PPU_SPRITES_MEMORY_SIZE             256  //In bytes
#define PPU_NUMBER_OF_SPRITES               64
#define PPU_MAX_NUMBER_OF_SPRITES_ON_LINE   8
#define PPU_SPRITE_OAM_CLEARING_BEGIN_XPOS  1
#define PPU_SPRITE_EVALUATION_BEGIN_XPOS    65
#define PPU_SPRITE_PRIORITY_BIT             0x80


#define PPU_VRAM_ADDR_VERT_INCREMENT_XPOS  256
#define PPU_VRAM_ADDR_HORIZONTAL_COPY_XPOS 257
#define PPU_BGND_FETCH_FOR_NEXT_LINE_XPOS  321


#define PPU_CYCLES_PER_FETCH_CYCLE         8
#define PPU_PATTERN_SIZE                   8

#define PPU_VIDEO_RAM_SIZE                 4096

#define PPU_SCREEN_PAGE1_BASE_ADDR          0x2000
#define PPU_SCREEN_PAGE_SYMBOLS_AREA_SIZE   960

#define PPU_CYCLES_PER_LINE                 341
#define PPU_CYCLES_PER_CPU_CYCLES           3
#define PPU_SET_VERT_BLANK_CYCLE            1
#define PPU_CLR_VERT_BLANK_CYCLE            1


#define PPU_SET_VBLANK_CYCLE                82183
#define PPU_CLEAR_VBLANK_SPRITE0_CYCLE      89003
#define PPU_FRAME_CYCLES                    89342

#define RENDERING_ENABLE                    1
#define BACKGROUND_FETCH_ENABLE             1
#define SPRITES_CLEAR_EVALUATE_FETCH_ENABLE 1


struct SpriteItems_t
{
    uint8_t Color               : 2;
    uint8_t                     : 3;
    uint8_t BgUpperSprite       : 1;
    uint8_t HorMirror           : 1;
    uint8_t VertMirror          : 1;
};

struct SpriteInfo_t
{
    uint8_t          yPos;
    uint8_t          ChrIdx;
    SpriteItems_t    Item;
    uint8_t          xPos;
};

struct SpriteFifo_t
{
    uint8_t          LowTile;
    uint8_t          HighTile;
};

typedef uint8_t (*PpuBusReadCallBack_t)      ( void* pContext, uint16_t busAddr );
typedef void    (*PpuBusWriteCallBack_t)     ( void* pContext, uint16_t busAddr, uint8_t busData );
typedef void    (*PpuPresentFrameCallBack_t) ( void* pContext, uint8_t* pData, uint16_t scanLine );
typedef void    (*PpuInterruptCallBack_t)    ( void* pContext );

union PpuBusAdress_t
{
    struct
    {
        uint16_t CoarseScrollX       : 5;
        uint16_t CoarseScrollY       : 5;
        uint16_t NameTableSelectHor  : 1;
        uint16_t NameTableSelectVert : 1;
        uint16_t FineScrollY         : 3;
        uint16_t                     : 1;
    };
          
    struct
    {
        uint16_t LowPart         : 8;
        uint16_t MiddlePart      : 6;
        uint16_t HidhPart        : 1;
        uint16_t                 : 1;         
    };

    struct
    {
        uint16_t                 : 11;
        uint16_t highV           :  4;
        uint16_t                 :  1;

    };

    struct
    {
        uint16_t                 : 6;

        uint16_t IsShift         : 1;
        uint16_t                 : 3;
        uint16_t NameTableSelect : 2;
    };

    uint16_t value               : 15;
};


union PpuShiftReg_t
{
    struct
    {
        uint16_t LowPart         : 8;
        uint16_t HidhPart        : 8;
    };

    uint16_t value;
};

class Ppu_t
{
public:
    // -------------- Initialization ----------------        
    void SetBusWriteCallBack( PpuBusWriteCallBack_t busWriteCallBack, void * pContext );
    void SetBusReadCallBack( PpuBusReadCallBack_t busReadCallBack, void * pContext );
    void SetPresentFrameCallBack( PpuPresentFrameCallBack_t presentFrameCallBack, void * pContext );
    void SetVsyncSignalCallBack( PpuInterruptCallBack_t vsyncCallBack, void * pContext );
    void SetLineCounterCallBack( PpuInterruptCallBack_t lineCounterCallBack, void * pContext );
    void Reset();
    //-----------------------------------------------
        
    //  ------ For Save Games ---------------
    void SavePpuContext( uint8_t* pOutData );
    void LoadPpuContext( uint8_t* pInData );
    // --------------------------------------

    // -----------------To CPU  --------------------
    uint8_t Read( uint16_t addr );
    void Write( uint16_t addr, uint8_t data );
    // ----------------------------------------------
        
    uint32_t Run( uint16_t cpuCycles );

           
private:
    inline uint8_t AssemlyPixel( uint8_t sprite, uint8_t bground );

private:
    inline void VideoRamAddrHorizontalInrement();
    inline void VideoRamAddrVerticalInrement();
    inline void VideoRamAddrHorizontalCopy();
    inline void VideoRamAddrVerticalCopy();
    inline void VideoRamBgFetch();

    inline void SpritesClearEvaluateFetch( uint16_t yPos );

private:
    inline uint8_t GetBackgroundPixel( uint16_t xPos );
    inline uint8_t GetSpritePixel( uint16_t xPos );
    inline void ReflectPalette( uint16_t addr, uint8_t value );

private:
    PpuRegisters_t m_PpuRegisters;
    PpuBusAdress_t m_CurrVideoRamAddr;
    PpuBusAdress_t m_TempVideoRamAddr;
    PpuShiftReg_t  m_BgLowTile;
    PpuShiftReg_t  m_BgHighTile;
    PpuShiftReg_t  m_SymbolItemByte;
    bool           m_FirstSecondWriteToggle;
    uint8_t        m_FineScrollXReg;
    uint8_t        m_FineScrollX;

private:
    uint8_t        m_PaletteBg     [ PPU_BG_PALETTE_SIZE ];
    uint8_t        m_PaletteSprites[ PPU_SPRITES_PALETTE_SIZE ];

private:
    SpriteInfo_t   m_PrimaryOam    [ PPU_NUMBER_OF_SPRITES ];
    SpriteInfo_t   m_SecondaryOam  [ PPU_MAX_NUMBER_OF_SPRITES_ON_LINE ];
    SpriteFifo_t   m_SpriteFifo    [ PPU_MAX_NUMBER_OF_SPRITES_ON_LINE ];

    uint8_t        m_ScanLine      [ PPU_HORIZONTAL_RESOLUTION ];
    uint8_t        m_AddrIncValue;
    uint8_t        m_SpriteSize;
    uint8_t        m_LastWritten;

private:    // Callbacks
    PpuBusWriteCallBack_t     m_fpBusWrite;
    PpuBusReadCallBack_t      m_fpBusRead;
    PpuPresentFrameCallBack_t m_fpPresentScanLine;
    PpuInterruptCallBack_t    m_fpVsyncSignal;
    PpuInterruptCallBack_t    m_fpIrqHook;
    void*                     m_pContext;
    void*                     m_pPresenFrameContext;

private:
    uint8_t                   m_IsOddFrame;
    uint16_t                  m_LineCounter;
    uint16_t                  m_PosX;
    uint32_t                  m_PpuCycles;
};

#endif
