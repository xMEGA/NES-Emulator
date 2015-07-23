#include "Ppu.h"

  PpuRegisters_t m_PpuRegisters;
    PpuBusAdress_t m_CurrVideoRamAddr;
    PpuBusAdress_t m_TempVideoRamAddr;
    bool           m_FirstSecondWriteToggle;
    //uint8_t        m_TempFineScrollX : 3;
    uint8_t        m_FineScrollX;// : 3;
    //uint8_t        m_T     : 5;

    uint8_t        m_BgScanLine  [ PPU_HORIZONTAL_RESOLUTION ];        
    uint8_t        m_PaletteBg   [ PPU_FONE_PALETTE_SIZE ];
    uint8_t        m_ScanLine      [ PPU_HORIZONTAL_RESOLUTION ];
    PpuShiftReg_t  m_BgLowTile;
    PpuShiftReg_t  m_BgHighTile;
    PpuShiftReg_t  m_SymbolItemByte; 


    uint8_t        m_AddrIncValue;
    uint8_t        m_SpriteSize;
    uint8_t        m_LastWritten;
    void*          m_pContext;
    void*          m_pPresenFrameContext;    


   // Call backs
    PpuBusWriteCallBack_t     fp_BusWriteCallBack;
    PpuBusReadCallBack_t      fp_BusReadCallBack;            
    PpuPresentFrameCallBack_t fp_PresentFrameCallBack;
    PpuInterruptCallBack_t    fp_VsyncSignalCallBack;
    PpuInterruptCallBack_t    fp_IrqHookCallBack;


    uint8_t                   m_IsOddFrame;
    uint16_t                  m_PosY;
    uint16_t                  m_PosX;
    
    uint32_t                  m_PpuCycles;

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

void Ppu_t::SavePpuContext( uint8_t* pOutData )
{
    uint8_t* pData = pOutData;
    uint8_t* pSource;


    *pData += m_PpuRegisters.AS;
    //*pData += m_PpuRegisters.DV;
    *pData += m_PpuRegisters.C1.Value;
    *pData += m_PpuRegisters.C2.Value;
    *pData += m_PpuRegisters.SR.Value;

    for( uint8_t i = 0; i != PPU_FONE_PALETTE_SIZE; i++ )
    {
        *pData ++= m_PaletteBg[ i ]; 
    }

    for( uint8_t i = 0; i != PPU_SPRITES_PALETTE_SIZE; i++ )
    {
        *pData ++= m_PaletteSprites[ i ]; 
    }

    pSource = reinterpret_cast< uint8_t* >( &m_PrimaryOam );

    for( uint16_t i = 0; i != PPU_SPRITES_MEMORY_SIZE; i++ )
    {
        *pData ++= *pSource++; 
    }  
}

void Ppu_t::LoadPpuContext( uint8_t* pInData )
{
    uint8_t* pData = pInData;
    uint8_t* pSource;

    m_PpuRegisters.AS                 = *pData++;
    //m_PpuRegisters.DV                 = *pData++;
    m_PpuRegisters.C1.Value           = *pData++;
    m_PpuRegisters.C2.Value           = *pData++;
    m_PpuRegisters.SR.Value           = *pData++;

    for( uint8_t i = 0; i != PPU_FONE_PALETTE_SIZE; i++ )
    {
        m_PaletteBg[ i ] = *pData ++; 
    }

    for( uint8_t i = 0; i != PPU_SPRITES_PALETTE_SIZE; i++ )
    {
        m_PaletteSprites[ i ] = *pData ++; 
    }

    pSource = reinterpret_cast< uint8_t* >( &m_PrimaryOam );

    for( uint16_t i = 0; i != PPU_SPRITES_MEMORY_SIZE; i++ )
    {
        *pSource++ = *pData ++; 
    }  
}

void Ppu_t::SetBusWriteCallBack( PpuBusWriteCallBack_t busWriteCallBack, void * pContext )
{
    fp_BusWriteCallBack = busWriteCallBack;
    m_pContext = pContext;
}

void Ppu_t::SetBusReadCallBack( PpuBusReadCallBack_t busReadCallBack, void * pContext )
{
    fp_BusReadCallBack = busReadCallBack;
    m_pContext = pContext;
}
    
void Ppu_t::SetPresentFrameCallBack( PpuPresentFrameCallBack_t presentFrameCallBack, void * pContext )
{
    fp_PresentFrameCallBack = presentFrameCallBack;
    m_pPresenFrameContext = pContext;
}

void Ppu_t::SetVsyncSignalCallBack( PpuInterruptCallBack_t vsyncCallBack, void * pContext )
{
    fp_VsyncSignalCallBack = vsyncCallBack;
    m_pContext = pContext;
}

void Ppu_t::SetLineCounterCallBack( PpuInterruptCallBack_t lineCounterCallBack, void * pContext )
{
    fp_IrqHookCallBack = lineCounterCallBack;
    m_pContext = pContext;
}
                       
void Ppu_t::Reset(void)
{
    m_PpuRegisters.C1.Value = 0;
    m_PpuRegisters.C2.Value = 0;
    m_PpuRegisters.AS       = 0;
   // m_PpuRegisters.DV       = 0;
    
    m_SpriteSize = 8;
    
    uint8_t* pData = ( uint8_t* )m_PrimaryOam;
    for( uint16_t i = 0; i < 256; i++ )
    {
        *pData++=0xFF;
    }

    m_PpuCycles   = 0;
    m_IsOddFrame  = SET_BIT;
    m_PosY = 0;
}

void Ppu_t::ReflectPalette( uint8_t value )
{
    switch( m_CurrVideoRamAddr.value & 0x000F )
    {
        case 0:
            m_PaletteSprites[ 0 ] = value;
            m_PaletteBg     [ 0 ] = value;
        break;
                
        case 4:
            m_PaletteSprites[ 4 ] = value;
            m_PaletteBg     [ 4 ] = value;
        break;

        case 8:
            m_PaletteSprites[ 8 ] = value;
            m_PaletteBg     [ 8 ] = value;
        break;

        case 12:
            m_PaletteSprites[ 12] = value;
            m_PaletteBg     [ 12] = value;
        break;
                
        default: break;
    }
}

uint8_t Ppu_t::Read(uint16_t addr)
{
     uint8_t* pTemp; 
     uint8_t  retValue = 0;

     addr &= 0x2007;

     switch (addr) 
     {
       case PPU_REG_STATUS_PPU_ADDR:
            m_PpuRegisters.SR.NotUsedBits = m_LastWritten;
            retValue = *( uint8_t *)&m_PpuRegisters.SR;
            m_PpuRegisters.SR.VsyncFlag = 0;
            m_FirstSecondWriteToggle = false;
       break;    
       
       case PPU_REG_DATA_SPRITES_ADDR:
            pTemp = (uint8_t *)&m_PrimaryOam;
            retValue = pTemp[ m_PpuRegisters.AS ];
       break;

       case PPU_REG_DATA_VRAM_ADDR:
            if( m_CurrVideoRamAddr.value < PPU_PALETTE_FONE_BASE_ADDR )
            {      
                retValue = fp_BusReadCallBack( m_pContext , m_PpuRegisters.AV );
                m_PpuRegisters.AV  = m_CurrVideoRamAddr.value;
            }
            else if( m_CurrVideoRamAddr.value < PPU_PALETTE_SPRITES_BASE_ADDR ) 
            {
                addr = m_CurrVideoRamAddr.value - PPU_PALETTE_FONE_BASE_ADDR + m_AddrIncValue;
                retValue = m_PaletteBg[ addr ];
            }
            else 
            {
                addr = m_CurrVideoRamAddr.value - PPU_PALETTE_SPRITES_BASE_ADDR + m_AddrIncValue;
                retValue = m_PaletteSprites[ addr ];
            }
            m_CurrVideoRamAddr.value += m_AddrIncValue;
       break;
     
       default:
       
       break;
     }
     return retValue;
}

void Ppu_t::Write(uint16_t addr, uint8_t data)
{
    uint8_t* pTemp; 
    m_LastWritten = data;
    
    addr &= 0x2007;

    switch ( addr ) 
    {
    case PPU_REG_CONTROL_1_ADDR:
        m_PpuRegisters.C1.Value = data;
                
        m_TempVideoRamAddr.NameTableSelect = data & 0x03;

        m_SpriteSize = 8;
        if ( m_PpuRegisters.C1.SpriteSize ) 
            m_SpriteSize = 16; 
            
        m_AddrIncValue = 1;               
        if ( m_PpuRegisters.C1.IncVideoRamAddr ) 
            m_AddrIncValue = 32;
           
    break;

    case PPU_REG_CONTROL_2_ADDR:
        m_PpuRegisters.C2.Value = data;         
    break;

    case PPU_REG_ADDR_SPRITES_ADDR:
        m_PpuRegisters.AS = data;
    break;

    case PPU_REG_DATA_SPRITES_ADDR:
        pTemp = ( uint8_t* )&m_PrimaryOam;
        pTemp[ m_PpuRegisters.AS ] = data;
        m_PpuRegisters.AS++;
    break;

    case PPU_REG_HARDWARE_SCROLL_ADDR:
        if( false == m_FirstSecondWriteToggle )
        {
            m_TempVideoRamAddr.CoarseScrollX = ( data >> 3 ) & 0x1F;
            m_FineScrollX                    = data & 0x07;
            m_FirstSecondWriteToggle         = true;
           
        }
        else
        {
            m_TempVideoRamAddr.CoarseScrollY = ( data >> 3 ) & 0x1F;
            m_TempVideoRamAddr.FineScrollY   = data & 0x07;
            m_FirstSecondWriteToggle         = false;
           
        }
    break;

    case PPU_REG_ADDR_VRAM_ADDR:  

        if( false == m_FirstSecondWriteToggle )
        {
            m_TempVideoRamAddr.MiddlePart = data & 0x3F;
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
        if( m_CurrVideoRamAddr.value < PPU_PALETTE_FONE_BASE_ADDR ) 
        {    
            fp_BusWriteCallBack( m_pContext ,  m_CurrVideoRamAddr.value, data );    
        }
        else if( m_CurrVideoRamAddr.value < PPU_PALETTE_SPRITES_BASE_ADDR ) 
        {
            addr = m_CurrVideoRamAddr.value - PPU_PALETTE_FONE_BASE_ADDR;
            m_PaletteBg   [ addr ] = data & 0x3F;
            ReflectPalette( data );
        }
        else 
        {
            addr =  m_CurrVideoRamAddr.value - PPU_PALETTE_SPRITES_BASE_ADDR;
            m_PaletteSprites[ addr ] = data  & 0x3F;
            ReflectPalette( data );
        }    
        m_CurrVideoRamAddr.value += m_AddrIncValue;
    break;
    }
}

//void Ppu_t::VideoRamAddrHorizontalInrement( void )
//{
//    if( 31 == m_CurrVideoRamAddr.CoarseScrollX )
//    {
//        //m_CurrVideoRamAddr.NameTableSelect++;
//        m_CurrVideoRamAddr.NameTableSelect ^= 1;
//    }
//   
//    m_CurrVideoRamAddr.CoarseScrollX++;
//   
//}
//
//void Ppu_t::VideoRamAddrVerticalInrement( void )
//{
//
//    if( 7 == m_CurrVideoRamAddr.FineScrollY )
//    {
//        m_CurrVideoRamAddr.CoarseScrollY++;
//    }
//    
//    m_CurrVideoRamAddr.FineScrollY ++;
//}

void Ppu_t::VideoRamAddrHorizontalInrement( void )
{
if( ( m_CurrVideoRamAddr.value & 0x001F ) == 31)
{
m_CurrVideoRamAddr.value &= ~0x001F;
m_CurrVideoRamAddr.value ^= 0x0400;
}
else
{
m_CurrVideoRamAddr.value++;
}
}
void Ppu_t::VideoRamAddrVerticalInrement( void )
{
if( ( m_CurrVideoRamAddr.value & 0x7000 ) != 0x7000 )
{
m_CurrVideoRamAddr.value += 0x1000;
}
else
{
m_CurrVideoRamAddr.value &= 0x0FFF;
int y = ( m_CurrVideoRamAddr.value & 0x03E0) >> 5;
if( y == 29 )
{
y = 0;
m_CurrVideoRamAddr.value ^= 0x0800;
}
else if( y == 31 )
{
y = 0;
}
else
{
y++;
}
m_CurrVideoRamAddr.value = ( m_CurrVideoRamAddr.value & ~0x03E0 ) | ( y << 5);
}
}


void Ppu_t::VideoRamAddrHorizontalCopy( void )
{
    m_CurrVideoRamAddr.CoarseScrollX = m_TempVideoRamAddr.CoarseScrollX;
    m_CurrVideoRamAddr.NameTableSelect &= ~0x01;
    m_CurrVideoRamAddr.NameTableSelect |= m_TempVideoRamAddr.NameTableSelect & 0x01;
}

void Ppu_t::VideoRamAddrVerticalCopy( void )
{
    m_CurrVideoRamAddr.CoarseScrollY = m_TempVideoRamAddr.CoarseScrollY;
    m_CurrVideoRamAddr.highV         = m_TempVideoRamAddr.highV;
}
   
 
#define PPU_PATTERN_SIZE 8


void Ppu_t::VideoRamBgFetch( void )
{
//    uint16_t nameTableTileAddr = PPU_SCREEN_PAGE1_BASE_ADDR | ( m_CurrVideoRamAddr.value & 0x0FFF );    
//    uint8_t patternIdx     = fp_BusReadCallBack( m_pContext, nameTableTileAddr );
//
//    uint16_t attributeAddr = ( PPU_SCREEN_PAGE1_BASE_ADDR + PPU_SCREEN_PAGE_SYMBOLS_AREA_SIZE ) | ( m_CurrVideoRamAddr.value & 0x0C00) | ( ( m_CurrVideoRamAddr.value >> 4 ) & 0x38 ) | ( ( m_CurrVideoRamAddr.value >> 2 ) & 0x07);
//                
//    m_SymbolItemByte.value >>= 8; 
//    m_SymbolItemByte.HidhPart = fp_BusReadCallBack( m_pContext, attributeAddr );   
//            
//    uint16_t videoRamAddr = ( patternIdx << 4 ) | ( uint16_t )( PPU_CHR1_ROM_BASE_ADDR * ( uint16_t )m_PpuRegisters.C1.BgChrBaseAddr ) | ( uint16_t )m_CurrVideoRamAddr.FineScrollY;
//
//  
//    //m_BgHighTile.value >>= 8;
//    //m_BgLowTile.value  >>= 8;
//  
//      
//    
//    m_BgLowTile.HidhPart  =  fp_BusReadCallBack( m_pContext, videoRamAddr );
//    m_BgHighTile.HidhPart =  fp_BusReadCallBack( m_pContext, videoRamAddr + 8 );
 
    
//    m_BgLowTile.LowPart  = m_BgLowTile.HidhPart;
//    m_BgHighTile.LowPart = m_BgHighTile.HidhPart;

    
}

bool test = false;

uint32_t Ppu_t::Run( uint16_t cpuCycles )
{
    uint32_t reqCycles = cpuCycles * 3;
    uint32_t returnCycles = reqCycles;

    for( uint32_t idx = 0; idx != reqCycles; idx++ )
    {
//        if( ( m_PpuCycles > PPU_SET_VBLANK_CYCLE ) && ( ( m_PpuCycles + reqCycles ) < PPU_CLEAR_VBLANK_SPRITE0_CYCLE ) ) // Iddle
//        {
//            m_PpuCycles += reqCycles;
//            break;
//        }

        if( m_PpuCycles >= PPU_FRAME_CYCLES )
        {
            m_PpuCycles = 0;
            m_IsOddFrame ^= SET_BIT;                      // Toggle Odd/Even frame

            if( SET_BIT == m_IsOddFrame )
            {
                m_PpuCycles ++;
            }
        }
        
        m_PosX = m_PpuCycles % 341;
        m_PosY = m_PpuCycles / 341;

        
      // ---------------------------------------------------------------------------*/

#if 1      
        
        if( test != m_PpuRegisters.C2.BgVisibleEnable )
        {
            test = m_PpuRegisters.C2.BgVisibleEnable;
        }
        
        if( ( m_PpuRegisters.C2.BgVisibleEnable ) && ( m_PpuRegisters.C2.BgVisibleRegion || ( m_PosX > 8 ) ) )
        {
           
            
            BgFetchRun();
            BgRenderingRun();
            
        }
        
       
        
        
        if( ( m_PosY <= 239 ) && ( m_PosX > 0 ) )
        {
            
            RunVisibleScanLine();

            if( m_PosX == 340 )
            {             

                if( fp_IrqHookCallBack )
                {
                    fp_IrqHookCallBack( m_pContext );
                }

                fp_PresentFrameCallBack( m_pPresenFrameContext, m_ScanLine, PPU_HORIZONTAL_RESOLUTION, PPU_HORIZONTAL_RESOLUTION * m_PosY );

            }

        }
#endif
      
// ---------------------------------------------------------------------------*/

	
        if( PPU_SET_VBLANK_CYCLE == m_PpuCycles )
        {
            m_PpuRegisters.SR.VsyncFlag = SET_BIT;
            
            if( m_PpuRegisters.C1.NmiRequestEnable )
            {
                fp_VsyncSignalCallBack( m_pContext );
            }
        }

        if( PPU_CLEAR_VBLANK_SPRITE0_CYCLE == m_PpuCycles )
        {
            m_PpuRegisters.SR.VsyncFlag          = CLR_BIT;
            m_PpuRegisters.SR.ZeroSpriteDetected = CLR_BIT;
        }

        m_PpuCycles ++;
	
    }
    

    return returnCycles;
}

void Ppu_t::RunVisibleScanLine( void )
{
    if( m_PpuRegisters.C2.SpritesVisibleEnable )
    {
        SpritesRun();
    }

    uint8_t yPosZeroSprite = m_PrimaryOam[0].yPos - 1;
    m_IsZeroSpriteOnCurrScanLine = CLR_BIT;
    
    if( ( m_PosY >= yPosZeroSprite) && ( m_PosY < ( yPosZeroSprite + m_SpriteSize /*+ 1*/) ) ) 
    {
        m_IsZeroSpriteOnCurrScanLine = SET_BIT;
    }


    uint16_t m_PosX =  m_PpuCycles % 341;
    
    
    
    
    if( m_PosX<= 256 )
    {
        uint16_t xVisible = m_PosX - 1;

        if( SET_BIT == m_IsZeroSpriteOnCurrScanLine )
        {
            if( ( xVisible >= m_PrimaryOam[0].xPos ) && ( xVisible <= ( m_PrimaryOam[0].xPos + 8 ) ) )
            {
                if( ( m_BgScanLine[xVisible] ) && ( m_SpriteScanLine[xVisible] ) )
                {
                    m_PpuRegisters.SR.ZeroSpriteDetected = SET_BIT;
                }
            }
        }

        uint8_t pixelColor = m_PaletteBg[0];
        uint8_t spriteColor =  m_SpriteScanLine[ xVisible ] & 0x0F;
        uint8_t backGround = m_BgScanLine [ xVisible ];

        if( 0 !=  m_SpriteScanLine[ xVisible ] ) 
        {
            pixelColor = m_PaletteSprites[ spriteColor ];
        }

        if( backGround ) 
        {
            pixelColor = m_PaletteBg[ backGround ];
        }
     
        if ( m_SpriteScanLine[ xVisible ] & PPU_SPRITE_PRIORITY_BIT )
        {
            pixelColor = m_PaletteSprites[ spriteColor ];
        }

        m_BgScanLine [ xVisible ] = 0;
        m_SpriteScanLine[ xVisible ] = 0;
        m_ScanLine[ xVisible ] = pixelColor;
            
    }

}


uint8_t PixelShiftRegisterLow  = 0;
uint8_t PixelShiftRegisterHigh = 0;


uint8_t bp = 0;
void Ppu_t::BgFetchRun( void )
{
    uint16_t xPos = m_PosX;
    uint16_t yPos = m_PosY;

  
    bool isBgFetcEnablePosition = ( ( ( xPos <= 256 ) || ( xPos > 320 ) ) && ( ( yPos < 240 ) || ( yPos > 260 ) ) ) ? true : false;
    
    if( 0 == xPos )
    {
        isBgFetcEnablePosition = false;
    }
    
    
    if( isBgFetcEnablePosition ) 
    {
    
        if( 0 == ( xPos % PPU_CYCLES_PER_FETCH_CYCLE ) )
        {
    
            if( xPos > 320 )
            {
                uint8_t bp = 0;
            }
            
            uint16_t nameTableTileAddr = PPU_SCREEN_PAGE1_BASE_ADDR | ( m_CurrVideoRamAddr.value & 0x0FFF );    
            uint8_t patternIdx     = fp_BusReadCallBack( m_pContext, nameTableTileAddr );

            uint16_t attributeAddr = ( PPU_SCREEN_PAGE1_BASE_ADDR + PPU_SCREEN_PAGE_SYMBOLS_AREA_SIZE ) | ( m_CurrVideoRamAddr.value & 0x0C00) | ( ( m_CurrVideoRamAddr.value >> 4 ) & 0x38 ) | ( ( m_CurrVideoRamAddr.value >> 2 ) & 0x07);

            //m_SymbolItemByte.value >>= 8; 
            m_SymbolItemByte.HidhPart = fp_BusReadCallBack( m_pContext, attributeAddr );   

            uint16_t videoRamAddr = ( patternIdx << 4 ) | ( uint16_t )( PPU_CHR1_ROM_BASE_ADDR * ( uint16_t )m_PpuRegisters.C1.BgChrBaseAddr ) | ( uint16_t )m_CurrVideoRamAddr.FineScrollY;

            
             
            //PixelShiftRegisterLow  = m_BgLowTile.LowPart;
            //PixelShiftRegisterHigh = m_BgHighTile.LowPart;    

//            m_BgHighTile.LowPart = m_BgHighTile.HidhPart;
//            m_BgLowTile.LowPart  = m_BgLowTile.HidhPart;


            m_BgLowTile.HidhPart  =  fp_BusReadCallBack( m_pContext, videoRamAddr );
            m_BgHighTile.HidhPart =  fp_BusReadCallBack( m_pContext, videoRamAddr + 8 );
            
            
            m_BgHighTile.LowPart = m_BgHighTile.HidhPart;
            m_BgLowTile.LowPart  = m_BgLowTile.HidhPart;

            
//            if( xPos > 320 )
//            {
//                m_BgHighTile.LowPart = m_BgHighTile.HidhPart;
//                m_BgLowTile.LowPart  = m_BgLowTile.HidhPart;
//            }
            
            VideoRamAddrHorizontalInrement();

            if( PPU_VRAM_ADDR_VERT_INCREMENT_XPOS == xPos )
            {
                VideoRamAddrVerticalInrement();
            }

        }
        
            
      

      
        

        uint8_t select = m_FineScrollX;

        uint8_t fonePixelColor  =( ( PixelShiftRegisterLow << select ) >> 7 ) & 0x01;
        fonePixelColor |=      ( ( ( PixelShiftRegisterHigh << select  )  >> 7 ) & 0x01 ) << 1;


        //m_SymbolItemByte.LowPart <<= 1;
        
        fonePixelColor += ( m_SymbolItemByte.HidhPart  ) & 0x0C;
        
        
        if( xPos < 336 )
        {
            PixelShiftRegisterHigh <<= 1;
            PixelShiftRegisterLow  <<= 1;

            PixelShiftRegisterLow  |= ( m_BgLowTile.LowPart  >> 0x07 ) & 0x01;
            PixelShiftRegisterHigh |= ( m_BgHighTile.LowPart >> 0x07 ) & 0x01;    

            m_BgLowTile.LowPart <<= 1;
            m_BgHighTile.LowPart <<= 1;
            
            
            //m_SymbolItemByte.HidhPart <<= 1;
            
        }
        

        if( fonePixelColor )
        {
            //uint16_t yScreenPage  = ( m_CurrVideoRamAddr.CoarseScrollY * 8 + m_CurrVideoRamAddr.FineScrollY ) % 240;
            //uint16_t xScreenPage  = m_T;       
            //uint8_t symbolItemMask  =  ( m_T / 16) + 2 * ( ( yScreenPage % 32 ) / 16 );
            uint8_t symbolItemValue = 0;// 0x03 & ( m_SymbolItemByte.LowPart >> (2 * symbolItemMask) );
            symbolItemValue <<= 2;
            fonePixelColor += symbolItemValue;
            m_BgScanLine[ xPos - 1  ] = fonePixelColor;
        }    
            
            
    }
    
    if( PPU_VRAM_ADDR_HORIZONTAL_COPY_XPOS == xPos )
    {
        VideoRamAddrHorizontalCopy();
    }

    
    if( ( m_PosY > 260 ) && ( ( xPos >= 280 ) && ( xPos <= 304 ) ) )
    {
        VideoRamAddrVerticalCopy();
    }
   

    //----------------------------------------------------------------------------------

    
    
    
}

void Ppu_t::BgRenderingRun( void ) 
{        
//    uint16_t xPos =  m_PosX;
//
//        
//    if( xPos % 8 == 0 )
//    {
//        m_BgLowTile.LowPart  = m_BgLowTile.HidhPart;
//        m_BgHighTile.LowPart = m_BgHighTile.HidhPart;
//    }
//    
//    if( ( xPos > 0 ) && ( ( xPos <= 256 ) || ( xPos > 320 ) ) )
//    {
//
//
//
//        //if( m_PpuRegisters.C2.BgVisibleRegion || ( xPos > 8 ) )
//        {
//            //uint8_t symbolLineOffset = 7 - m_FineScrollX;
//
//
//            PixelShiftRegisterLow  |= ( m_BgLowTile.LowPart  >> 0x07 ) & 0x01;
//            PixelShiftRegisterHigh |= ( m_BgHighTile.LowPart >> 0x07 ) & 0x01;    
//
//
//            m_BgLowTile.LowPart <<= 1;
//            m_BgHighTile.LowPart <<= 1;
//
//
//            uint8_t select = m_FineScrollX;
//
//            uint8_t fonePixelColor  =( ( PixelShiftRegisterLow << select ) >> 7 ) & 0x01;
//            fonePixelColor |=  ( ( ( PixelShiftRegisterHigh << select )  >> 7) & 0x01 ) << 1;
//
//            
//           // uint8_t fonePixelColor  =(  m_BgLowTile.LowPart >> 7 ) & 0x01;
//            //fonePixelColor |=  ( (  m_BgHighTile.LowPart  >> 7) & 0x01 ) << 1;
//            
//            //m_BgLowTile.LowPart <<= 1;
//            //m_BgHighTile.LowPart <<= 1;
//
//            PixelShiftRegisterHigh <<= 1;
//            PixelShiftRegisterLow  <<= 1;
//
//
//
//            if( fonePixelColor ) // Если пиксель не прозрачный
//            {
//                //uint16_t yScreenPage  = ( m_CurrVideoRamAddr.CoarseScrollY * 8 + m_CurrVideoRamAddr.FineScrollY ) % 240;
//                //uint16_t xScreenPage  = m_T;       
//                //uint8_t symbolItemMask  =  ( m_T / 16) + 2 * ( ( yScreenPage % 32 ) / 16 );
//                uint8_t symbolItemValue = 0;// 0x03 & ( m_SymbolItemByte.LowPart >> (2 * symbolItemMask) );
//                symbolItemValue <<= 2;
//                fonePixelColor += symbolItemValue;
//                m_BgScanLine[ xPos  ] = fonePixelColor;
//            }
//
//        }
//
//    }
//    //----------------------------------------------------------------------------------

    
}

void Ppu_t::SpritesRun( void ) 
{
    uint16_t nextScanLine = m_PosY + 0;
  
    uint16_t xPos =  m_PpuCycles % 341;
//    for( uint16_t xPos = m_LineCycles; xPos < ( m_LineCycles + m_CyclesToExecute ); xPos++ ) 
    {
        
        if( ( 0 != xPos ) && ( xPos <= PPU_HORIZONTAL_RESOLUTION ) )
        {
            m_SpriteScanLine[xPos] = 0;

            uint8_t nVisibleSprite = 0xFF;

            for ( int8_t nRenderSprite = 7; nRenderSprite >= 0; nRenderSprite-- ) // Поиск номера спрайта, который нужно отобразить
            {
                if ( (  0xFF != m_SpriteFifo[nRenderSprite].xPos ) && ( xPos >= m_SpriteFifo[nRenderSprite].xPos ) && ( xPos < ( m_SpriteFifo[nRenderSprite].xPos + 8 ) ) )
                {
                    nVisibleSprite = nRenderSprite;
            
                }
            
                if( nVisibleSprite != 0xFF )
                {
                    if( m_SpriteFifo[nVisibleSprite].xPos == xPos )
                    {
                        m_SpritePixel = 0;
                    }

                    uint8_t spriteLineOffset = 7 - m_SpritePixel;
                    if( m_SpriteFifo[nVisibleSprite].HorMirror ) 
                        spriteLineOffset = m_SpritePixel;

                    uint8_t spritePixelColor  = ( m_SpriteFifo[nVisibleSprite].LowTile >> spriteLineOffset ) & 0x01;
                    spritePixelColor |= ( ( m_SpriteFifo[nVisibleSprite].HighTile >> spriteLineOffset ) & 0x01) << 1;

                    if (spritePixelColor > 0) 
                    {
                        spritePixelColor |= m_SpriteFifo[nVisibleSprite].Color << 2;

                        m_SpriteScanLine[xPos] = spritePixelColor;

                        if( 0 == m_SpriteFifo[nVisibleSprite].Priority )
                        {
                            m_SpriteScanLine[xPos] |= PPU_SPRITE_PRIORITY_BIT;
                        }   

                    }    


                }
            
        }

    m_SpritePixel++;
    }
        
        if( ( 0 != xPos ) && ( xPos <= PPU_SPRITE_OAM_CLEARING_END_XPOS ) ) // Clearing Secondary OAM
        {
            if( PPU_SPRITE_OAM_CLEARING_BEGIN_XPOS == xPos ) 
            {
                m_pSecondaryOam = ( uint8_t* )m_SecondaryOam;
            }

            if( xPos <= 32 )
            {
                *m_pSecondaryOam ++= 0xFF;
            }
        }
        else if( xPos <= PPU_SPRITE_EVALUATION_END_XPOS ) // Sprites evaluation
        {
            if( xPos == PPU_SPRITE_EVALUATION_BEGIN_XPOS ) 
            {
                m_PpuRegisters.SR.SpriteCountPerLine = 0;
                m_SpriteIdx   = 0;
                m_EvalSpriteIdx = 0;
            }

            if( ( 0 == xPos % 3 ) && ( PPU_MAX_NUMBER_OF_SPRITES_ON_LINE > m_EvalSpriteIdx ) )
            {
                SpriteInfo_t spriteInfo = m_PrimaryOam[m_SpriteIdx];
            
                if ( ( nextScanLine >= spriteInfo.yPos ) && ( nextScanLine < (spriteInfo.yPos + m_SpriteSize) ) ) // Предвыборка спрайтов, которые находятся на следующей скан-линии
                {
                    m_SecondaryOam[m_EvalSpriteIdx] = spriteInfo;               
                    m_EvalSpriteIdx ++;
                
                    
                    m_IsZeroSpriteOnNextScanLine = CLR_BIT;

                    if( 0 == m_SpriteIdx ) // Если на следующей скан-линии находится 0-й спрайт
                    {
                        m_IsZeroSpriteOnNextScanLine = SET_BIT;
                    }

                    if( PPU_MAX_NUMBER_OF_SPRITES_ON_LINE == m_EvalSpriteIdx ) 
                    {
                        m_PpuRegisters.SR.SpriteCountPerLine = SET_BIT;
                    }
                }
        
                m_SpriteIdx++;
            }
        }
        else if( xPos <= PPU_SPRITE_FETCH_END_XPOS )// Копируем из Secondary OAM в FIFO
        {
            if( PPU_SPRITE_FETCH_BEGIN_XPOS == xPos ) 
            {
                m_SpriteIdx = 0;
            }

            if( 0 == xPos % 8 )
            {
                SpriteInfo_t spriteInfo = m_SecondaryOam[m_SpriteIdx];
                uint8_t yPixelOfSprite = nextScanLine - spriteInfo.yPos;
                uint8_t spriteIdx = spriteInfo.ChrIdx; 
        
                uint16_t spritesChrBaseAddr = PPU_CHR1_ROM_BASE_ADDR * m_PpuRegisters.C1.SpriteChrBaseAddr;
                if( m_SpriteSize == 16 ) 
                {
                    spritesChrBaseAddr = PPU_CHR1_ROM_BASE_ADDR * ( spriteIdx % 2 );
                    spriteIdx &= 0xFE;
                
                    if ( spriteInfo.Item.VertMirror ) 
                        yPixelOfSprite = 15 - yPixelOfSprite;
                    
                    if ( yPixelOfSprite > 7) 
                        spriteIdx++;

                    yPixelOfSprite &= 7;
                } 
                else 
                {    
                    if( spriteInfo.Item.VertMirror ) 
                        yPixelOfSprite = 7 - yPixelOfSprite;
                }

                uint16_t spriteSymbolRamAddr = spritesChrBaseAddr + yPixelOfSprite + spriteIdx * PPU_CHR_SYMBOL_SIZE;

                m_SpriteFifo[m_SpriteIdx].xPos      = spriteInfo.xPos;
                m_SpriteFifo[m_SpriteIdx].HorMirror = spriteInfo.Item.HorMirror;
                m_SpriteFifo[m_SpriteIdx].Priority  = spriteInfo.Item.BgUpperSprite;
                m_SpriteFifo[m_SpriteIdx].Color     = spriteInfo.Item.Color;
                m_SpriteFifo[m_SpriteIdx].LowTile   = fp_BusReadCallBack( m_pContext, spriteSymbolRamAddr );
                m_SpriteFifo[m_SpriteIdx].HighTile  = fp_BusReadCallBack( m_pContext, spriteSymbolRamAddr + 8 );
    
                m_IsZeroSpriteOnCurrScanLine = m_IsZeroSpriteOnNextScanLine;
                    
                m_SpriteIdx++;
            }
        }
    }
}