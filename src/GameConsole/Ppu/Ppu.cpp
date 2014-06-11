#include "Ppu.h"

// Примечание:
// Символ - блок изображения размером 8 x 8, для формирования фона
// Спрайт - блок изображения размером 8 x 8, или 8 x 16, который обрабатывается независимо от основного изображения ( фона )


uint8_t regH = 0;
uint8_t regL = 0;
uint8_t regA = 0;

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
        *pData ++= m_PaletteFone[ i ]; 
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
        m_PaletteFone[ i ] = *pData ++; 
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
    m_LineCounter = 0;
}

void Ppu_t::ReflectPalette( uint8_t value )
{
    switch( m_CurrVideoRamAddr.value & 0x000F )
    {
        case 0:
            m_PaletteSprites[ 0 ] = value;
            m_PaletteFone   [ 0 ] = value;
        break;
                
        case 4:
            m_PaletteSprites[ 4 ] = value;
            m_PaletteFone   [ 4 ] = value;
        break;

        case 8:
            m_PaletteSprites[ 8 ] = value;
            m_PaletteFone   [ 8 ] = value;
        break;

        case 12:
            m_PaletteSprites[ 12] = value;
            m_PaletteFone   [ 12] = value;
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
                retValue = m_PaletteFone[ addr ];
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
            m_PaletteFone   [ addr ] = data & 0x3F;
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
   
 
void Ppu_t::VideoRamFoneFetch( void )
{
    uint16_t videoRamAddr = PPU_SCREEN_PAGE1_BASE_ADDR | ( m_CurrVideoRamAddr.value & 0x0FFF );    
    uint8_t symbolIdx     = fp_BusReadCallBack( m_pContext, videoRamAddr );

    m_FoneHighTile.value >>= 8;
    m_FoneLowTile.value  >>= 8;
    m_SymbolItemByte.value >>= 8;  
        
    videoRamAddr = PPU_SCREEN_PAGE1_BASE_ADDR + PPU_SCREEN_PAGE_SYMBOLS_AREA_SIZE;
    videoRamAddr |= ( m_CurrVideoRamAddr.value & 0x0C00) | ( ( m_CurrVideoRamAddr.value >> 4 ) & 0x38 ) | ( ( m_CurrVideoRamAddr.value >> 2 ) & 0x07);
            
    m_SymbolItemByte.HidhPart = fp_BusReadCallBack( m_pContext, videoRamAddr );   
            
    //symbolIdx = 1;

    videoRamAddr = ( symbolIdx << 4 ) + ( uint16_t )( PPU_CHR1_ROM_BASE_ADDR * ( uint16_t )m_PpuRegisters.C1.FoneChrBaseAddr ) | ( uint16_t )m_CurrVideoRamAddr.FineScrollY;

    m_FoneLowTile.HidhPart  =  fp_BusReadCallBack( m_pContext, videoRamAddr );
    m_FoneHighTile.HidhPart =  fp_BusReadCallBack( m_pContext, videoRamAddr + 8 );
           
}





uint32_t Ppu_t::Run( uint16_t cpuCycles )
{
    uint32_t reqCycles = cpuCycles * 3;
    uint32_t returnCycles = reqCycles;

    for( uint16_t idx = 0; idx != reqCycles; idx++ )
    {
        m_PpuCycles ++;

        if( m_PpuCycles >= 89342 )
        {
            m_PpuCycles = 0;
            m_IsOddFrame ^= SET_BIT;                      // Toggle Odd/Even frame
            if( SET_BIT == m_IsOddFrame )
            {
                continue;
            }
        }


        m_LineCounter = m_PpuCycles / 341;

        

        if( m_LineCounter <= 239 )
        {
            uint16_t xPos =  m_PpuCycles % 341;

            if( 0 != xPos )
            {
                RunVisibleScanLine();

                if( xPos == 340 )
                {             
        
                    if( fp_IrqHookCallBack )
                    {
                        fp_IrqHookCallBack( m_pContext );
                    }

                    fp_PresentFrameCallBack( m_pPresenFrameContext, m_ScanLine, PPU_HORIZONTAL_RESOLUTION, PPU_HORIZONTAL_RESOLUTION * m_LineCounter );

                }

            }
        }
        else if( 241 == m_LineCounter )
        {
            uint16_t xPos =  m_PpuCycles % 341;

            if( PPU_SET_VERT_BLANK_CYCLE == xPos ) // Если выполняем цикл PPU (от начала линии), на котором нужно установить VsyncFlag
            {
                m_PpuRegisters.SR.VsyncFlag = SET_BIT;
            
                if( m_PpuRegisters.C1.NmiRequestEnable )
                {
                    fp_VsyncSignalCallBack( m_pContext );
                }
            
            }    
        }
        else if( m_LineCounter == 261 )
        {
            if( m_PpuRegisters.C2.FoneVisibleEnable )
            {
                RunPreRenderLine();
            }

            uint16_t xPos =  m_PpuCycles % 341;

            if( PPU_CLR_VERT_BLANK_CYCLE == xPos )
            {
                m_PpuRegisters.SR.VsyncFlag          = CLR_BIT;
                m_PpuRegisters.SR.ZeroSpriteDetected = CLR_BIT;
            }
        }



    }

    return returnCycles;
}


void Ppu_t::RunVisibleScanLine( void )
{
    if( m_PpuRegisters.C2.FoneVisibleEnable )
    {
        FoneRun();
    }

    if( m_PpuRegisters.C2.SpritesVisibleEnable )
    {
        SpritesRun();
    }

    uint8_t yPosZeroSprite = m_PrimaryOam[0].yPos - 1;
    m_IsZeroSpriteOnCurrScanLine = CLR_BIT;
    
    if( ( m_LineCounter >= yPosZeroSprite) && ( m_LineCounter < ( yPosZeroSprite + m_SpriteSize /*+ 1*/) ) ) 
    {
        m_IsZeroSpriteOnCurrScanLine = SET_BIT;
    }


    uint16_t xPos =  m_PpuCycles % 341;
    
    if( xPos<= PPU_HORIZONTAL_RESOLUTION )
    {
        uint16_t xVisible = xPos - 1;

        if( SET_BIT == m_IsZeroSpriteOnCurrScanLine )
        {
            if( ( xVisible >= m_PrimaryOam[0].xPos ) && ( xVisible <= ( m_PrimaryOam[0].xPos + 8 ) ) )
            {
                if( ( m_FoneScanLine[xVisible] ) && ( m_SpriteScanLine[xVisible] ) )
                {
                    m_PpuRegisters.SR.ZeroSpriteDetected = SET_BIT;
                }
            }
        }

        uint8_t pixelColor = m_PaletteFone[0];
        uint8_t spriteColor =  m_SpriteScanLine[ xVisible ] & 0x0F;
        uint8_t backGround = m_FoneScanLine [ xVisible ];

        if( 0 !=  m_SpriteScanLine[ xVisible ] ) 
        {
            pixelColor = m_PaletteSprites[ spriteColor ];
        }

        if( backGround ) 
        {
            pixelColor = m_PaletteFone[ backGround ];
        }
     
        if ( m_SpriteScanLine[ xVisible ] & PPU_SPRITE_PRIORITY_BIT )
        {
            pixelColor = m_PaletteSprites[ spriteColor ];
        }

        m_FoneScanLine [ xVisible ] = 0;
        m_SpriteScanLine[ xVisible ] = 0;
        m_ScanLine[ xVisible ] = pixelColor;
            
    }

}

void Ppu_t::RunPreRenderLine( void )
{

     uint16_t xPos =  m_PpuCycles % 341;
     //for( uint16_t xPos = m_LineCycles; xPos < ( m_LineCycles + m_CyclesToExecute ); xPos++ ) 
     {
       // if( m_PpuRegisters.C2.FoneVisibleRegion || ( xPos > 8 ) )
        {
            if( ( xPos <= PPU_HORIZONTAL_RESOLUTION ) ) 
            {
                if( ( 0 == ( xPos % PPU_CYCLES_PER_FETCH_CYCLE ) ) && ( 0 != xPos ) )
                {
                    VideoRamFoneFetch();    
                    VideoRamAddrHorizontalInrement();
               
                    if( PPU_VRAM_ADDR_VERT_INCREMENT_XPOS == xPos )
                    {
                        VideoRamAddrVerticalInrement();
                    }
                
                }       

            }
            else if( PPU_VRAM_ADDR_HORIZONTAL_COPY_XPOS == xPos )
            {
                VideoRamAddrHorizontalCopy();
            }
            else if( PPU_BGND_FETCH_FOR_NEXT_LINE_XPOS <= xPos )
            {      
                if( 0 == ( xPos % PPU_CYCLES_PER_FETCH_CYCLE ) )
                {
                    VideoRamFoneFetch();
                    VideoRamAddrHorizontalInrement();
                }
            }

          /*  if( PPU_CLR_VERT_BLANK_CYCLE == xPos )
            {
                m_PpuRegisters.SR.VsyncFlag          = CLR_BIT;
                m_PpuRegisters.SR.ZeroSpriteDetected = CLR_BIT;
            }*/
            if( ( xPos >= 280 ) && ( xPos <= 304 ) )
            {
                VideoRamAddrVerticalCopy();
            }


        }
     }
}

void Ppu_t::FoneRun( void ) 
{        
    uint16_t xPos =  m_PpuCycles % 341;
//     for( uint16_t xPos = m_LineCycles; xPos < ( m_LineCycles + m_CyclesToExecute ); xPos++ ) 
     {
        
        // ----------------------------------------------------------------
        

        if( ( 0 != xPos ) &&  ( xPos <= PPU_HORIZONTAL_RESOLUTION ) )
        {
            
            
             
            if( m_PpuRegisters.C2.FoneVisibleRegion || ( xPos > 8 ) )
            {
                uint8_t symbolLineOffset = 7 - m_Pixel;
                uint8_t fonePixelColor  = ( regL >> symbolLineOffset ) & 0x01;
                fonePixelColor |=  ( ( regH >> symbolLineOffset ) & 0x01) << 1;

            
                if( fonePixelColor ) // Если пиксель не прозрачный
                {
                    uint16_t yScreenPage  = ( m_CurrVideoRamAddr.CoarseScrollY * 8 + m_CurrVideoRamAddr.FineScrollY ) % 240;
                    //uint16_t xScreenPage  = m_T;       
                    uint8_t symbolItemMask  =  ( m_T / 16) + 2 * ( ( yScreenPage % 32 ) / 16 );
                    uint8_t symbolItemValue = 0x03 & ( regA >> (2 * symbolItemMask) );
                    symbolItemValue <<= 2;
                    fonePixelColor += symbolItemValue;
                    m_FoneScanLine[ xPos - 1 ] = fonePixelColor;
                }
                
            }

            m_Pixel ++;      
            m_T++;

           if( 0 == ( xPos % PPU_CYCLES_PER_FETCH_CYCLE ) || (m_Pixel == 0) )
            {       
                    
                regH = m_FoneHighTile.HidhPart;
                regL = m_FoneLowTile.HidhPart;    
                regA = m_SymbolItemByte.HidhPart;  
            }
                      

        }
        else
        {
                regH = m_FoneHighTile.LowPart;
                regL = m_FoneLowTile.LowPart;
                regA = m_SymbolItemByte.LowPart;
                  
                if( 327 == xPos )
                {
                    m_Pixel = m_FineScrollX;
                    m_T = ( (( m_CurrVideoRamAddr.CoarseScrollX ) << 3 ) | m_FineScrollX );  
                }

        }
        //----------------------------------------------------------------------------------

    

        if( ( xPos <= PPU_HORIZONTAL_RESOLUTION ) ) 
        {
            if( ( 0 == ( xPos % PPU_CYCLES_PER_FETCH_CYCLE ) ) && ( 0 != xPos ) )
            {
                VideoRamFoneFetch();    
                VideoRamAddrHorizontalInrement();
               
                if( PPU_VRAM_ADDR_VERT_INCREMENT_XPOS == xPos )
                {
                    VideoRamAddrVerticalInrement();
                }
                
            }       

        }
        else if( PPU_VRAM_ADDR_HORIZONTAL_COPY_XPOS == xPos )
        {
            VideoRamAddrHorizontalCopy();
        }
        else if( PPU_BGND_FETCH_FOR_NEXT_LINE_XPOS <= xPos )
        {      
            if( 0 == ( xPos % PPU_CYCLES_PER_FETCH_CYCLE ) )
            {
                VideoRamFoneFetch();
                VideoRamAddrHorizontalInrement();
            }
        }
    }
}

void Ppu_t::SpritesRun( void ) 
{
    uint16_t nextScanLine = m_LineCounter + 0;
  
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
                            else
                            {
//                            uint8_t a = 0;
                            }
                    
                        }    
                        
                        
                    }
                    

            //    }
            //}
            
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
                m_SpriteFifo[m_SpriteIdx].Priority  = spriteInfo.Item.FoneUpperSprite;
                m_SpriteFifo[m_SpriteIdx].Color     = spriteInfo.Item.Color;
                m_SpriteFifo[m_SpriteIdx].LowTile   = fp_BusReadCallBack( m_pContext, spriteSymbolRamAddr );
                m_SpriteFifo[m_SpriteIdx].HighTile  = fp_BusReadCallBack( m_pContext, spriteSymbolRamAddr + 8 );
    
                m_IsZeroSpriteOnCurrScanLine = m_IsZeroSpriteOnNextScanLine;
                    
                m_SpriteIdx++;
            }
        }
    }
}