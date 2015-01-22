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
#define PRE_SET_BIT    2

#define PPU_SPRITES_PALETTE_SIZE            16        // Размер палитры спрайтов (в байтах )
#define PPU_FONE_PALETTE_SIZE               16        // Размер палитры фона (в байтах )

#define PPU_PALETTE_FONE_BASE_ADDR          0x3F00    // Начальный адресс палитры фона
#define PPU_PALETTE_SPRITES_BASE_ADDR       0x3F10    // Начальный адресс палитры спрайтов


//#define PPU_CHR0_ROM_BASE_ADDR              0x0000    // Начальный адрес знакогенератора 0
#define PPU_CHR1_ROM_BASE_ADDR              0x1000    // Начальный адрес знакогенератора 1


#define PPU_HORIZONTAL_RESOLUTION           256       // Горизонтальное разрешение экрана ( для системы PAL )
#define PPU_VERTICAL_PAL_RESOLUTION         240       // Вертикальное разрешение экрана ( для системы PAL )


#define PPU_CHR_SYMBOL_HEIGHT               8         // Высота символа из знакогенератора (в пикселях)
#define PPU_CHR_SYMBOL_WIDTH                8         // Ширина символа из знакогенератора (в пикселях)
#define PPU_CHR_SYMBOL_SIZE                 16        // Размер символа из знакогенератора в памяти (в байтах)


#define PPU_SPRITES_MEMORY_SIZE             256       // Размер памяти спрайтов
#define PPU_NUMBER_OF_SPRITES               64
#define PPU_MAX_NUMBER_OF_SPRITES_ON_LINE   8
#define PPU_SPRITE_OAM_CLEARING_BEGIN_XPOS  1
#define PPU_SPRITE_OAM_CLEARING_END_XPOS    64
#define PPU_SPRITE_EVALUATION_BEGIN_XPOS    65
#define PPU_SPRITE_EVALUATION_END_XPOS      256
#define PPU_SPRITE_FETCH_BEGIN_XPOS         257
#define PPU_SPRITE_FETCH_END_XPOS           320
#define PPU_SPRITE_PRIORITY_BIT             0x80



#define PPU_VRAM_ADDR_VERT_INCREMENT_XPOS  256
#define PPU_VRAM_ADDR_HORIZONTAL_COPY_XPOS 257
#define PPU_BGND_FETCH_FOR_NEXT_LINE_XPOS  321


#define PPU_CYCLES_PER_FETCH_CYCLE         8


#define PPU_VIDEO_RAM_SIZE                   4096

#define PPU_SCREEN_PAGE1_BASE_ADDR          0x2000
#define PPU_SCREEN_PAGE_SYMBOLS_AREA_SIZE   960


//#define PPU_SCREEN_PAGE2_BASE_ADDR          0x2400    // Начальный адрес второй экранной страници
//#define PPU_SCREEN_PAGE3_BASE_ADDR          0x2800    // Начальный адрес третьей экранной страници
//#define PPU_SCREEN_PAGE4_BASE_ADDR          0x2C00    // Начальный адрес четвертой экранной страници
//#define PPU_SCREEN_PAGE_SIZE                0x0400    
//#define PPU_VISIBLE_SCAN_LINE_LENGTH        256       // Длина видимой области скан-линии ( в пикселях )
//#define PPU_NON_VISIBLE_SCAN_LINE_LENGTH    85        // Длина невидимой области скан-линии ( в пикселях )
//#define PPU_PRE_RENDERING_SCAN_LINES        1         // Количество скан линий между ??? и следующим кадром
//#define PPU_VISIBLE_SCAN_LINES              240       // Количество скан видимых скан - линий
//#define PPU_PAL_POST_RENDERING_SCAN_LINES   4         // Количество скан-линий после окончания прорисовки картинки до формирования NMI
//#define PPU_PAL_SCAN_LINES_AFTER_NMI        20        // Количество скан-линий после формирования NMI до момента сброса флага NMI
//#define PPU_NUMBER_OF_SYMBOLS_IN_LINE       32      // Максимальное количество знакомест в строке  
//#define PPU_SCREEN_PAGE_SIZE                1024


#define PPU_CYCLES_PER_LINE                 341
#define PPU_SET_VERT_BLANK_CYCLE            1         // Номер цикла ( от начала линии ) на во время которого нужно установить VsyncFlag 
#define PPU_CLR_VERT_BLANK_CYCLE            1         // Номер цикла ( от начала линии ) на во время которого нужно сбросить VsyncFlag 


#define PPU_SET_VBLANK_CYCLE            82183
#define PPU_CLEAR_VBLANK_SPRITE0_CYCLE  89003
#define PPU_FRAME_CYCLES                89342

enum PpuState_t
{
    PPU_VISIBLE_SCAN_LINES_STATE     = 0,
    PPU_POST_RENDER_SCAN_LINES_STATE    ,
    PPU_VERT_BLANK_LINE_STATE           ,
    PPU_PRE_RENDER_SCAN_LINE_STATE
};

struct SpriteItems_t
{
    uint8_t Color               : 2;
    uint8_t                     : 3;
    uint8_t FoneUpperSprite     : 1;
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
    uint8_t          xPos;
    uint8_t          LowTile;
    uint8_t          HighTile;
    uint8_t          Priority  : 1;
    uint8_t          Color     : 2;
    uint8_t          HorMirror : 1;
    uint8_t                    : 4;
};

typedef uint8_t (*PpuBusReadCallBack_t)      ( void* pContext, uint16_t busAddr );
typedef void    (*PpuBusWriteCallBack_t)     ( void* pContext, uint16_t busAddr, uint8_t busData );
typedef void    (*PpuPresentFrameCallBack_t) ( void* pContext, uint8_t* pData, uint16_t len, uint16_t posInFrame );
typedef void    (*PpuInterruptCallBack_t)    ( void* pContext );

union PpuBusAdress_t
{
    struct
    {
        uint16_t CoarseScrollX   : 5;
        uint16_t CoarseScrollY   : 5;
        uint16_t NameTableSelect : 2;
        uint16_t FineScrollY     : 3;
        uint16_t                 : 1;
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

        uint16_t value           : 15;
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
    void Reset(void);
    //-----------------------------------------------
        
    //  ------ For Save Games ---------------
    void SavePpuContext( uint8_t* pOutData );
    void LoadPpuContext( uint8_t* pInData );
    // --------------------------------------

    // -----------------To CPU  --------------------
    uint8_t Read(uint16_t addr);
    void Write(uint16_t addr, uint8_t data);
    // ----------------------------------------------
        
    uint32_t Run( uint16_t cpuCycles );

           
private:
    inline void RunVisibleScanLine  ( void );
    inline void RunPreRenderLine    ( void );

private:
    inline void VideoRamAddrHorizontalInrement( void );
    inline void VideoRamAddrVerticalInrement( void );
    inline void VideoRamAddrHorizontalCopy( void );
    inline void VideoRamAddrVerticalCopy( void );
    inline void VideoRamFoneFetch( void );

private:
    inline void FoneRun( void );
    inline void SpritesRun( void );
    inline void ReflectPalette( uint8_t value );

private:
    PpuRegisters_t m_PpuRegisters;
    PpuBusAdress_t m_CurrVideoRamAddr;
    PpuBusAdress_t m_TempVideoRamAddr;
    bool           m_FirstSecondWriteToggle;
    uint8_t        m_FineScrollX : 3;
    uint8_t        m_Pixel : 3;
    uint8_t        m_T     : 5;

private:
    uint8_t        m_FoneScanLine  [ PPU_HORIZONTAL_RESOLUTION ];        
    uint8_t        m_PaletteFone   [ PPU_FONE_PALETTE_SIZE ];
    uint8_t        m_ScanLine      [ PPU_HORIZONTAL_RESOLUTION ];
    PpuShiftReg_t  m_FoneLowTile;
    PpuShiftReg_t  m_FoneHighTile;
    PpuShiftReg_t  m_SymbolItemByte; 


    uint8_t        m_AddrIncValue;
    uint8_t        m_SpriteSize;
    uint8_t        m_LastWritten;
    void*          m_pContext;
    void*          m_pPresenFrameContext;    


private:    // Call backs
    PpuBusWriteCallBack_t     fp_BusWriteCallBack;
    PpuBusReadCallBack_t      fp_BusReadCallBack;            
    PpuPresentFrameCallBack_t fp_PresentFrameCallBack;
    PpuInterruptCallBack_t    fp_VsyncSignalCallBack;
    PpuInterruptCallBack_t    fp_IrqHookCallBack;

private:
    uint8_t                   m_IsOddFrame;
    uint16_t                  m_LineCounter;
    uint32_t                  m_PpuCycles;
private:
    SpriteInfo_t              m_PrimaryOam    [ PPU_NUMBER_OF_SPRITES ];
    SpriteInfo_t              m_SecondaryOam  [ PPU_MAX_NUMBER_OF_SPRITES_ON_LINE ];
    SpriteFifo_t              m_SpriteFifo    [ PPU_MAX_NUMBER_OF_SPRITES_ON_LINE ];
    uint8_t                   m_SpriteScanLine[ PPU_HORIZONTAL_RESOLUTION ];
    uint8_t                   m_PaletteSprites[ PPU_SPRITES_PALETTE_SIZE ];
    uint8_t                   m_EvalSpriteIdx;             
    uint8_t                   m_SpriteIdx;
    uint8_t                   m_SpritePixel;
    uint8_t*                  m_pSecondaryOam;
    bool                      m_IsZeroSpriteOnNextScanLine;
    bool                      m_IsZeroSpriteOnCurrScanLine;
};

#endif