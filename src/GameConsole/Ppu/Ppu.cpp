#include "Ppu.h"
#include <string.h>

void Ppu_t::SavePpuContext( uint8_t* pOutData )
{
    UNUSED( pOutData );
}

void Ppu_t::LoadPpuContext( uint8_t* pInData )
{
    UNUSED( pInData );
}

void Ppu_t::SetBusWriteCallBack( PpuBusWriteCallBack_t busWriteCallBack, void* pContext )
{
    m_fpBusWrite = busWriteCallBack;
    m_pContext = pContext;
}

void Ppu_t::SetBusReadCallBack( PpuBusReadCallBack_t busReadCallBack, void* pContext )
{
    m_fpBusRead = busReadCallBack;
    m_pContext = pContext;
}

void Ppu_t::SetPresentFrameCallBack( PpuPresentFrameCallBack_t presentFrameCallBack, void* pContext )
{
    m_fpPresentScanLine = presentFrameCallBack;
    m_pPresenFrameContext = pContext;
}

void Ppu_t::SetVsyncSignalCallBack( PpuInterruptCallBack_t vsyncCallBack, void* pContext )
{
    m_fpVsyncSignal = vsyncCallBack;
    m_pContext = pContext;
}

void Ppu_t::SetLineCounterCallBack( PpuInterruptCallBack_t lineCounterCallBack, void * pContext )
{
    m_fpIrqHook = lineCounterCallBack;
    m_pContext = pContext;
}

void Ppu_t::Reset()
{
    m_PpuRegisters.C1.Value = 0;
    m_PpuRegisters.C2.Value = 0;
    m_PpuRegisters.AS       = 0;
    // m_PpuRegisters.DV      = 0;

    memset( m_PrimaryOam, 0xFF, sizeof( m_PrimaryOam ) );

    m_PpuCycles   = 0;
    m_IsOddFrame  = SET_BIT;
    m_SpriteSize  = PPU_PATTERN_SIZE;
    m_LineCounter = 0;
    m_PosX        = 0;
}

void Ppu_t::ReflectPalette( uint16_t addr, uint8_t value )
{
    addr &= 0x000F;

    switch( addr )
    {
        case 0x00:
        case 0x04:
        case 0x08:
        case 0x0C   :
            m_PaletteSprites[ addr ] = value;
            m_PaletteBg[ addr ] = value;
        break;

        default: break;
    }
}

uint8_t Ppu_t::Read( uint16_t addr )
{
    uint8_t  retValue = 0;

    //addr &= 0x2007;

    switch( addr )
    {
        case PPU_REG_STATUS_PPU_ADDR:
           // m_PpuRegisters.SR.NotUsedBits = m_LastWritten;
            retValue = m_PpuRegisters.SR.Value;
            m_PpuRegisters.SR.VsyncFlag = 0;
            m_FirstSecondWriteToggle = false;
        break;

        case PPU_REG_DATA_SPRITES_ADDR:
        {
            uint8_t* pPrimaryOam = ( uint8_t* )&m_PrimaryOam;
            retValue = pPrimaryOam[ m_PpuRegisters.AS ];
        }
        break;

        case PPU_REG_DATA_VRAM_ADDR:
            if( m_CurrVideoRamAddr.value < PPU_PALETTE_BG_BASE_ADDR)
            {
                retValue = m_fpBusRead(m_pContext, m_PpuRegisters.AV);
                m_PpuRegisters.AV = m_CurrVideoRamAddr.value;
            }
            else if( m_CurrVideoRamAddr.value < PPU_PALETTE_SPRITES_BASE_ADDR)
            {
                addr = m_CurrVideoRamAddr.value - PPU_PALETTE_BG_BASE_ADDR + m_AddrIncValue;
                retValue = m_PaletteBg[ addr ];
            }
            else
            {
                addr = m_CurrVideoRamAddr.value - PPU_PALETTE_SPRITES_BASE_ADDR + m_AddrIncValue;
                retValue = m_PaletteSprites[ addr ];
            }
            m_CurrVideoRamAddr.value += m_AddrIncValue;
        break;

        default: break;
    }
    return retValue;
}

void Ppu_t::Write(uint16_t addr, uint8_t data)
{
    m_LastWritten = data;

    //addr &= 0x2007;

    switch( addr )
    {
    case PPU_REG_CONTROL_1_ADDR:
        m_PpuRegisters.C1.Value = data;
        m_TempVideoRamAddr.NameTableSelect = data;
        m_SpriteSize   = ( 0 != m_PpuRegisters.C1.SpriteSize ) ? 16 : 8;
        m_AddrIncValue = ( 0 != m_PpuRegisters.C1.IncVideoRamAddr ) ? 32 : 1;
    break;

    case PPU_REG_CONTROL_2_ADDR:
        m_PpuRegisters.C2.Value = data;
    break;

    case PPU_REG_ADDR_SPRITES_ADDR:
        m_PpuRegisters.AS = data;
    break;

    case PPU_REG_DATA_SPRITES_ADDR:
    {
        uint8_t* pPrimaryOam = ( uint8_t* )&m_PrimaryOam;
        pPrimaryOam[ m_PpuRegisters.AS ] = data;
        m_PpuRegisters.AS++;
    }
    break;

    case PPU_REG_HARDWARE_SCROLL_ADDR:

        if( false == m_FirstSecondWriteToggle )
        {
            m_TempVideoRamAddr.CoarseScrollX = data >> 3;
            m_FineScrollXReg = data & 0x07;
            m_FirstSecondWriteToggle = true;

        }
        else
        {
            m_TempVideoRamAddr.CoarseScrollY = data >> 3;
            m_TempVideoRamAddr.FineScrollY   = data;
            m_FirstSecondWriteToggle = false;

        }

    break;

    case PPU_REG_ADDR_VRAM_ADDR:

        if( false == m_FirstSecondWriteToggle )
        {
            m_TempVideoRamAddr.MiddlePart = data;
            m_TempVideoRamAddr.HidhPart   = 0;
            m_FirstSecondWriteToggle      = true;
        }
        else
        {
            m_TempVideoRamAddr.LowPart = data;
            m_CurrVideoRamAddr.value   = m_TempVideoRamAddr.value;
            m_FirstSecondWriteToggle   = false;
        }

    break;

    case PPU_REG_DATA_VRAM_ADDR:

        if( m_CurrVideoRamAddr.value < PPU_PALETTE_BG_BASE_ADDR )
        {
            m_fpBusWrite( m_pContext, m_CurrVideoRamAddr.value, data );
        }
        else if( m_CurrVideoRamAddr.value < PPU_PALETTE_SPRITES_BASE_ADDR )
        {
            addr = m_CurrVideoRamAddr.value - PPU_PALETTE_BG_BASE_ADDR;
            m_PaletteBg[ addr ] = data & 0x3F;
            ReflectPalette( m_CurrVideoRamAddr.value, data );
        }
        else
        {
            addr = m_CurrVideoRamAddr.value - PPU_PALETTE_SPRITES_BASE_ADDR;
            m_PaletteSprites[ addr ] = data & 0x3F;
            ReflectPalette( m_CurrVideoRamAddr.value, data );
        }
        m_CurrVideoRamAddr.value += m_AddrIncValue;

    break;
    }
}

void Ppu_t::VideoRamAddrHorizontalInrement()
{
    if( 31 == m_CurrVideoRamAddr.CoarseScrollX )
    {
        m_CurrVideoRamAddr.CoarseScrollX = 0;
        m_CurrVideoRamAddr.NameTableSelectHor ^= 1;           // switch horizontal nametable
    }
    else
    {
        m_CurrVideoRamAddr.value++;                // increment coarse X
    }
}

void Ppu_t::VideoRamAddrVerticalInrement()
{
    if( ( m_CurrVideoRamAddr.value & 0x7000 ) != 0x7000 ) //if( (m_CurrVideoRamAddr.FineScrollY ) < 7 )
    {
        m_CurrVideoRamAddr.FineScrollY++;
    }
    else
    {
        m_CurrVideoRamAddr.FineScrollY = 0;

        int coarseY = m_CurrVideoRamAddr.CoarseScrollY; // let coarseY = CoarseY

        if( coarseY != m_CurrVideoRamAddr.CoarseScrollY )
        {
            coarseY++;
        }

        if( 29 == coarseY )
        {
            coarseY = 0;                                   // coarse Y = 0
            m_CurrVideoRamAddr.NameTableSelectVert ^= 1;   // switch vertical nametable
        }
        else if( 31 == coarseY )
        {
            coarseY = 0;                          // coarse Y = 0, nametable not switched
        }
        else
        {
            coarseY ++;                           // increment coarse Y
        }

        m_CurrVideoRamAddr.CoarseScrollY = coarseY;     // put coarse Y back into v
    }
}

void Ppu_t::VideoRamAddrHorizontalCopy()
{
    m_CurrVideoRamAddr.CoarseScrollX      = m_TempVideoRamAddr.CoarseScrollX;
    m_CurrVideoRamAddr.NameTableSelectHor = m_TempVideoRamAddr.NameTableSelectHor;
}

void Ppu_t::VideoRamAddrVerticalCopy()
{
    m_CurrVideoRamAddr.CoarseScrollY = m_TempVideoRamAddr.CoarseScrollY;
    m_CurrVideoRamAddr.highV         = m_TempVideoRamAddr.highV;
}

void Ppu_t::VideoRamBgFetch()
{
    uint16_t nameTableTileAddr = PPU_SCREEN_PAGE1_BASE_ADDR | ( m_CurrVideoRamAddr.value & 0x0FFF );

    uint8_t patternIdx = m_fpBusRead(m_pContext, nameTableTileAddr);

    uint16_t attributeAddr = 0x23C0 | (m_CurrVideoRamAddr.value & 0x0C00) | ((m_CurrVideoRamAddr.value >> 4) & 0x38) | ((m_CurrVideoRamAddr.value >> 2) & 0x07);

    //uint16_t attributeAddr = (PPU_SCREEN_PAGE1_BASE_ADDR + PPU_SCREEN_PAGE_SYMBOLS_AREA_SIZE) | (m_CurrVideoRamAddr.value & 0x0C00) | ((m_CurrVideoRamAddr.value >> 4) & 0x38) | ((m_CurrVideoRamAddr.value >> 2) & 0x07);
    //uint16_t attributeAddr = PPU_SCREEN_PAGE1_BASE_ADDR | PPU_SCREEN_PAGE_SYMBOLS_AREA_SIZE | ( m_CurrVideoRamAddr.NameTableSelect) | ( ( m_CurrVideoRamAddr.value >> 4 ) & 0x38 ) | ( ( m_CurrVideoRamAddr.value >> 2 ) & 0x07);

    uint16_t videoRamAddr = ( patternIdx << 4 ) | ( uint16_t )( PPU_CHR1_ROM_BASE_ADDR * ( uint16_t )m_PpuRegisters.C1.BgChrBaseAddr ) | ( uint16_t )m_CurrVideoRamAddr.FineScrollY;

    m_SymbolItemByte.HidhPart = m_fpBusRead( m_pContext, attributeAddr );
    m_BgLowTile.LowPart       =  m_fpBusRead( m_pContext, videoRamAddr );
    m_BgHighTile.LowPart      =  m_fpBusRead( m_pContext, videoRamAddr + PPU_PATTERN_SIZE );

}
 
uint32_t Ppu_t::Run( uint16_t cpuCycles )
{
    uint32_t reqCycles = cpuCycles * PPU_CYCLES_PER_CPU_CYCLES;
    uint32_t returnCycles = reqCycles;

    for( uint32_t idx = 0; idx < reqCycles; idx++ )
    {

        if( ( m_PpuCycles > PPU_SET_VBLANK_CYCLE ) && ( ( m_PpuCycles + reqCycles ) < PPU_CLEAR_VBLANK_SPRITE0_CYCLE ) ) // Iddle
        {
            m_PpuCycles += reqCycles;
            m_PosX += reqCycles;

            if( m_PosX >= PPU_CYCLES_PER_LINE )
            {
                m_LineCounter++;
                m_PosX -= PPU_CYCLES_PER_LINE;
            }

            break;
        }

        if( m_PpuCycles >= PPU_FRAME_CYCLES )
        {
            m_PpuCycles = 0;
            m_LineCounter = 0;
            m_IsOddFrame ^= SET_BIT;                      // Toggle Odd/Even frame

            if( ( SET_BIT == m_IsOddFrame ) && m_PpuRegisters.C2.BgVisibleEnable && m_PpuRegisters.C2.SpritesVisibleEnable )
            {
                m_PpuCycles++;
                returnCycles--;
            }

            m_PosX = m_PpuCycles;
        }


#if RENDERING_ENABLE

        if( m_LineCounter <= 239 )
        {
            if( ( m_PosX <= PPU_HORIZONTAL_RESOLUTION ) && ( m_PosX > 0 ) )
            {

                uint16_t xVisible = m_PosX - 1;

                uint8_t spriteColor = m_PpuRegisters.C2.SpritesVisibleEnable * GetSpritePixel( xVisible );
                uint8_t bgColor = m_PpuRegisters.C2.BgVisibleEnable * GetBackgroundPixel( xVisible );

                if( CLR_BIT == m_PpuRegisters.SR.ZeroSpriteDetected )
                {
                    uint8_t yPosZeroSprite = m_PrimaryOam[ 0 ].yPos;
                    uint8_t xPosZeroSprite = m_PrimaryOam[ 0 ].xPos;

                    uint16_t yPos = m_LineCounter;

                    if( ( yPos >= yPosZeroSprite ) && ( yPos < ( yPosZeroSprite + m_SpriteSize ) ) )
                    {
                        if( ( xVisible >= xPosZeroSprite) && ( xVisible <= ( xPosZeroSprite + PPU_PATTERN_SIZE ) ) )
                        {
                            if( ( ( bgColor > 0 ) || ( 0 == m_PrimaryOam[ 0 ].Item.BgUpperSprite ) ) && ( spriteColor ) )
                            {
                                m_PpuRegisters.SR.ZeroSpriteDetected = SET_BIT;
                            }
                        }
                    }
                }

                uint8_t pixelColor = AssemlyPixel( spriteColor, bgColor );

                m_ScanLine[ xVisible ] = pixelColor;
            }
            else if( 329 == m_PosX )
            {
                if( m_PpuRegisters.C2.BgVisibleEnable )
                {
                    m_BgHighTile.HidhPart    = m_BgHighTile.LowPart;
                    m_BgLowTile.HidhPart     = m_BgLowTile.LowPart;
                    m_SymbolItemByte.LowPart = m_SymbolItemByte.HidhPart;
                    m_FineScrollX = m_FineScrollXReg;
                }

                if( NULL != m_fpIrqHook )
                {
                     m_fpIrqHook( m_pContext );
                }

                m_fpPresentScanLine( m_pPresenFrameContext, m_ScanLine, m_LineCounter );
            }
        }

#endif


#if BACKGROUND_FETCH_ENABLE

        if( ( m_LineCounter <= 239) || (261 == m_LineCounter ) )
        {
            if( m_PpuRegisters.C2.BgVisibleEnable)
            {
                if( (m_PosX <= PPU_HORIZONTAL_RESOLUTION ) && ( 0 != m_PosX ) )
                {
                    if( 0 == ( m_PosX % PPU_CYCLES_PER_FETCH_CYCLE ) )
                    {
                        VideoRamBgFetch();
                        VideoRamAddrHorizontalInrement();

                        if( PPU_VRAM_ADDR_VERT_INCREMENT_XPOS == m_PosX )
                        {
                            VideoRamAddrVerticalInrement();
                        }

                    }
                }
                else if( PPU_VRAM_ADDR_HORIZONTAL_COPY_XPOS == m_PosX )
                {
                    VideoRamAddrHorizontalCopy();
                }
                else if(  ( m_PosX >= PPU_BGND_FETCH_FOR_NEXT_LINE_XPOS ) && ( m_PosX <= 336 ) )
                {
                    if( 0 == ( m_PosX % PPU_CYCLES_PER_FETCH_CYCLE ) )
                    {
                        VideoRamBgFetch();
                        VideoRamAddrHorizontalInrement();
                    }
                }

                if( ( 261 == m_LineCounter ) && ( m_PosX >= 280 ) && ( m_PosX <= 304 ) )
                {
                    VideoRamAddrVerticalCopy();
                }
            }

#if SPRITES_CLEAR_EVALUATE_FETCH_ENABLE

            if( PPU_SPRITE_OAM_CLEARING_BEGIN_XPOS == m_PosX )
            {
                SpritesClearEvaluateFetch( m_LineCounter );
            }
#endif
        }

#endif


        if( PPU_SET_VBLANK_CYCLE == m_PpuCycles )
        {
            m_PpuRegisters.SR.VsyncFlag = SET_BIT;

            if( 0 != m_PpuRegisters.C1.NmiRequestEnable )
            {
                m_fpVsyncSignal( m_pContext );
            }
        }

        if( PPU_CLEAR_VBLANK_SPRITE0_CYCLE == m_PpuCycles )
        {
            m_PpuRegisters.SR.VsyncFlag = CLR_BIT;
            m_PpuRegisters.SR.ZeroSpriteDetected = CLR_BIT;
        }

        m_PpuCycles++;
        m_PosX++;

        if( m_PosX >= PPU_CYCLES_PER_LINE )
        {
            m_PosX = 0;
            m_LineCounter++;
        }
    }

    return returnCycles;
}


uint8_t Ppu_t::AssemlyPixel( uint8_t sprite, uint8_t bground )
{
    uint8_t pixelColor  = m_PaletteBg[ 0 ];
    uint8_t spriteColor = sprite & 0x0F;
    uint8_t backGround  = bground;

    if( sprite & PPU_SPRITE_PRIORITY_BIT )
    {
        pixelColor = m_PaletteSprites[ spriteColor ];
    }
    else
    {
        if( 0 != sprite )
        {
            pixelColor = m_PaletteSprites[ spriteColor ];
        }

        if( 0 != backGround )
        {
            pixelColor = m_PaletteBg[ backGround ];
        }
    }

    return pixelColor;
}

uint8_t Ppu_t::GetBackgroundPixel( uint16_t xPos )
{
    uint8_t fonePixelColor = 0;
    uint8_t finePixelPos = xPos % 8;

//    uint8_t tmp  = ( ( finePixelPos + ( 8 * m_CurrVideoRamAddr.CoarseScrollX ) % 32 + m_FineScrollX + 16  ) % 32 ) / 16;
    uint8_t tmp  = ( ( finePixelPos + ( ( ( m_CurrVideoRamAddr.CoarseScrollX + /*-*/ 2 )  & 0x03 ) << 3 ) + m_FineScrollXReg ) & 0x1F ) >> 4;

    if(  0 == tmp % PPU_PATTERN_SIZE )
    {
        m_SymbolItemByte.LowPart = m_SymbolItemByte.HidhPart;
    }

    if( m_PpuRegisters.C2.BgVisibleRegion || ( xPos > PPU_PATTERN_SIZE ) )
    {
        uint8_t symbolLineOffset = 7 - m_FineScrollX;// - m_Pixel;

        fonePixelColor = (m_BgLowTile.HidhPart >> symbolLineOffset) & 0x01;
        fonePixelColor |= ((m_BgHighTile.HidhPart >> symbolLineOffset) & 0x01) << 1;

        if( 0 != fonePixelColor )
        {
            uint8_t symbolItemMask = 2 * ( m_CurrVideoRamAddr.IsShift * 2 + tmp );
            uint8_t symbolItemValue = 0x03 & m_SymbolItemByte.LowPart >> symbolItemMask;

            symbolItemValue <<= 2;
            fonePixelColor += symbolItemValue;
        }
    }

    m_BgLowTile.value  <<= 1;
    m_BgHighTile.value <<= 1;

    return fonePixelColor;
}

void Ppu_t::SpritesClearEvaluateFetch( uint16_t yPos )
{
    uint16_t nextScanLine = yPos - 1;

    memset( m_SecondaryOam, 0xFF, sizeof( m_SecondaryOam ) ); // Clearing Secondary OAM

    m_PpuRegisters.SR.SpriteCountPerLine = 0;
    uint32_t evalSpriteIdx = 0;

    for( uint32_t idx = 0; idx < PPU_NUMBER_OF_SPRITES; idx++ ) // Sprites evaluation
    {
        SpriteInfo_t spriteInfo = m_PrimaryOam[idx];

        if( ( nextScanLine >= spriteInfo.yPos ) && ( nextScanLine < ( spriteInfo.yPos + m_SpriteSize ) ) )
        {
            m_SecondaryOam[ evalSpriteIdx ] = spriteInfo;
            evalSpriteIdx++;

            if( PPU_MAX_NUMBER_OF_SPRITES_ON_LINE == evalSpriteIdx )
            {
                m_PpuRegisters.SR.SpriteCountPerLine = SET_BIT;

                break; //Array overflow protect
            }
        }
    }

    for( uint32_t idx = 0; idx < PPU_MAX_NUMBER_OF_SPRITES_ON_LINE; idx++ ) // Copy from Secondary OAM into FIFO
    {
        SpriteInfo_t spriteInfo = m_SecondaryOam[idx];
        uint8_t yPixelOfSprite = nextScanLine - spriteInfo.yPos;
        uint8_t spriteIdx = spriteInfo.ChrIdx;

        uint16_t spritesChrBaseAddr = PPU_CHR1_ROM_BASE_ADDR * m_PpuRegisters.C1.SpriteChrBaseAddr;

        if( 0 == m_PpuRegisters.C1.SpriteSize )
        {
            if( spriteInfo.Item.VertMirror )
            {
                yPixelOfSprite = 7 - yPixelOfSprite;
            }
        }
        else
        {
            spritesChrBaseAddr = PPU_CHR1_ROM_BASE_ADDR * (spriteIdx % 2);
            spriteIdx &= 0xFE;

            if( spriteInfo.Item.VertMirror )
                yPixelOfSprite = 15 - yPixelOfSprite;

            if( yPixelOfSprite > 7)
                spriteIdx++;

            yPixelOfSprite &= 7;
        }

        uint16_t spriteSymbolRamAddr = spritesChrBaseAddr + yPixelOfSprite + spriteIdx * PPU_CHR_SYMBOL_SIZE;

        m_SpriteFifo[ idx ].LowTile   = m_fpBusRead( m_pContext, spriteSymbolRamAddr );
        m_SpriteFifo[ idx ].HighTile  = m_fpBusRead( m_pContext, spriteSymbolRamAddr + 8 );
    }
}

uint8_t Ppu_t::GetSpritePixel( uint16_t xPos )
{
    uint8_t retValue = 0;

    for (int8_t nRenderSprite = 0; nRenderSprite < 8; nRenderSprite++) //!!!
    {
        SpriteInfo_t* pOam = &m_SecondaryOam[ nRenderSprite ];

        if( ( xPos >= pOam->xPos ) && ( xPos < ( pOam->xPos + 8 ) ) )
        {
            uint8_t spritePixel = xPos - pOam->xPos;

            uint8_t spriteLineOffset = 7 - spritePixel;

            if( pOam->Item.HorMirror)
            {
                spriteLineOffset = spritePixel;
            }

            SpriteFifo_t* pSprite = &m_SpriteFifo[ nRenderSprite ];

            uint8_t spritePixelColor = (pSprite->LowTile >> spriteLineOffset) & 0x01;
            spritePixelColor |= ((pSprite->HighTile >> spriteLineOffset) & 0x01) << 1;

            if( spritePixelColor > 0)
            {
                spritePixelColor |= pOam->Item.Color << 2;

                retValue = spritePixelColor;

                if( 0 == pOam->Item.BgUpperSprite)
                {
                    retValue |= PPU_SPRITE_PRIORITY_BIT;
                }

                break;
            }
        }
    }

    return retValue;
}
