/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Cpu.h"
#include "CpuRegisters.h"

static uint8_t CycTable[] =
{
    /*0x00*/ 7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6, 
    /*0x10*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
    /*0x20*/ 6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, 
    /*0x30*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
    /*0x40*/ 6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6, 
    /*0x50*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
    /*0x60*/ 6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6, 
    /*0x70*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
    /*0x80*/ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, 
    /*0x90*/ 2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5, 
    /*0xA0*/ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, 
    /*0xB0*/ 2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4, 
    /*0xC0*/ 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, 
    /*0xD0*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
    /*0xE0*/ 2, 6, 3, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, 
    /*0xF0*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7,4,4,7,7,
};

uint32_t Cpu_t::DumpRegisters( uint8_t* pOutRegistersData )
{
    uint8_t* pData = pOutRegistersData;

    *pData ++= m_Registers.AC;
    *pData ++= m_Registers.X;
    *pData ++= m_Registers.Y;
    *pData ++= m_Registers.SP;
    *pData ++= m_Registers.SR.value;
    *pData ++= m_Registers.PC.lowPart;
    *pData ++= m_Registers.PC.highPart;

    return pData - pOutRegistersData;
}

uint32_t Cpu_t::LoadRegisters( const uint8_t* pInRegistersData )
{
    const uint8_t* pData = pInRegistersData;

    m_Registers.AC          = *pData++;
    m_Registers.X           = *pData++;
    m_Registers.Y           = *pData++;
    m_Registers.SP          = *pData++;
    m_Registers.SR.value    = *pData++;
    m_Registers.PC.lowPart  = *pData++;
    m_Registers.PC.highPart = *pData++;

    return pData - pInRegistersData;
}


#define BusWrite( addr, value )\
    fp_BusWriteCallBack( m_Context, addr, value );

#define BusRead( addr )\
    fp_BusReadCallBack( m_Context, addr )

uint32_t Cpu_t::DumpMemory( uint8_t* pData, uint16_t startAddr, uint32_t bytesCnt )
{
    
    for( uint32_t idx = 0; idx < bytesCnt; idx++ )
    {
        pData[ idx ] = BusRead( startAddr + idx ); 
    }
    
    return bytesCnt;
}

uint32_t Cpu_t::LoadToMemory( const uint8_t* pData, uint16_t startAddr, uint32_t bytesCnt )
{
    
    for( uint32_t idx = 0; idx < bytesCnt; idx++ )
    {
        fp_BusWriteCallBack( m_Context, startAddr + idx, pData[ idx ] ); 
    }
    
    return bytesCnt;
}

void Cpu_t::SetBusWriteCallBack( CpuBusWriteCallBack_t busWriteCallBack, void * pContext )
{
    m_Context = pContext;
    fp_BusWriteCallBack = busWriteCallBack;
}

void Cpu_t::SetBusReadCallBack( CpuBusReadCallBack_t busReadCallBack, void * pContext )
{
    m_Context = pContext;
    fp_BusReadCallBack = busReadCallBack;
}

#define CalcZeroFlag( value )  m_Registers.SR.Z = ( value & 0x00FF ) ? CLR_BIT : SET_BIT
#define CalcCarryFlag( value ) m_Registers.SR.C = ( value & 0xFF00 ) ? SET_BIT : CLR_BIT;
#define CalcSignFlag( value )  m_Registers.SR.N = ( value & 0x0080 ) ? SET_BIT : CLR_BIT;
#define CalcOverflowFlag( execRes, acc, mem )\
    m_Registers.SR.V = ( ( execRes ^ acc ) & ( execRes ^ mem ) & 0x0080 ) ? SET_BIT : CLR_BIT;

#define Dex \
        m_Registers.X--; \
        CalcZeroFlag( m_Registers.X ); \
        CalcSignFlag( m_Registers.X );

#define Dey \
        m_Registers.Y--; \
        CalcZeroFlag( m_Registers.Y ); \
        CalcSignFlag( m_Registers.Y );

#define Inx \
        m_Registers.X++; \
        CalcZeroFlag( m_Registers.X ); \
        CalcSignFlag( m_Registers.X );

#define Iny \
        m_Registers.Y++; \
        CalcZeroFlag( m_Registers.Y ); \
        CalcSignFlag( m_Registers.Y );
    
#define Pla \
        m_Registers.AC = StackPop(); \
        CalcZeroFlag( m_Registers.AC ); \
        CalcSignFlag( m_Registers.AC );
    
#define Rti \
        m_Registers.SR.value    = StackPop(); \
        m_Registers.PC.lowPart  = StackPop(); \
        m_Registers.PC.highPart = StackPop(); 
    
#define Rts \
        uint16_t operand = StackPop(); \
        operand |= StackPop() << 8; \
        m_Registers.PC.value = operand + 1;
    
#define Tax \
        m_Registers.X = m_Registers.AC; \
        CalcZeroFlag( m_Registers.X ); \
        CalcSignFlag( m_Registers.X );

#define Tay \
        m_Registers.Y = m_Registers.AC; \
        CalcZeroFlag( m_Registers.Y ); \
        CalcSignFlag( m_Registers.Y );

#define Tsx \
        m_Registers.X = m_Registers.SP; \
        CalcZeroFlag( m_Registers.X ); \
        CalcSignFlag( m_Registers.X );

#define Txa \
        m_Registers.AC = m_Registers.X; \
        CalcZeroFlag( m_Registers.AC ); \
        CalcSignFlag( m_Registers.AC );

#define Tya \
        m_Registers.AC = m_Registers.Y; \
        CalcZeroFlag( m_Registers.AC ); \
        CalcSignFlag( m_Registers.AC );

#define Ldy \
    m_Registers.Y = ( uint8_t )operandValue; \
    CalcZeroFlag( m_Registers.Y ); \
    CalcSignFlag( m_Registers.Y ); \
    if( true == isAdditionCycleEnable ) \
        cycles++;

#define Jsr \
      m_Registers.PC.value--;\
      StackPush( m_Registers.PC.highPart );\
      StackPush( m_Registers.PC.lowPart );\
      m_Registers.PC.value = operandAddr.value;

#define Ror\
    uint32_t executeResult = operandValue >> 1;\
    executeResult |= m_Registers.SR.C << 7;/*!!!*/\
    m_Registers.SR.C = ( operandValue & 0x01 ) ? SET_BIT : CLR_BIT;\
    CalcZeroFlag( executeResult );\
    CalcSignFlag( executeResult );\
    uint8_t saveValue = ( uint8_t )executeResult;

#define Sbc\
    operandValue ^= 0x00FF;\
    uint32_t executeResult = ( uint16_t )m_Registers.AC + operandValue + ( uint16_t )m_Registers.SR.C;\
    CalcCarryFlag( executeResult );\
    CalcZeroFlag( executeResult );\
    CalcOverflowFlag( executeResult, m_Registers.AC, operandValue );\
    CalcSignFlag( executeResult );\
    if( true == isAdditionCycleEnable )\
        cycles++;\
    m_Registers.AC = ( uint8_t )executeResult;    
    
#define Brk\
    m_Registers.PC.value++;\
    StackPush( m_Registers.PC.highPart );\
    StackPush( m_Registers.PC.lowPart );\
    m_Registers.SR.B = SET_BIT;\
    StackPush( m_Registers.SR.value );\
    m_Registers.SR.I = SET_BIT;\
    m_Registers.PC.lowPart   = BusRead( 0xFFFE );\
    m_Registers.PC.highPart  = BusRead( 0xFFFF );    
    
#define Inc\
    operandValue ++;\
    CalcZeroFlag( operandValue );\
    CalcSignFlag( operandValue );\
    uint8_t saveValue = ( uint8_t )operandValue;
        
#define Lda\
    m_Registers.AC = ( uint8_t )operandValue;\
    CalcZeroFlag( m_Registers.AC );\
    CalcSignFlag( m_Registers.AC );\
    if( true == isAdditionCycleEnable )\
        cycles++;
        
#define Ldx\
    m_Registers.X = ( uint8_t )operandValue;\
    CalcZeroFlag( m_Registers.X );\
    CalcSignFlag( m_Registers.X );\
   /* if( m_AdditionCycleEnable )
    {
        cycles++;
    }*/
        
#define Lsr\
    uint32_t executeResult = operandValue >> 1;\
    m_Registers.SR.C = (operandValue & 1)  ? SET_BIT : CLR_BIT;\
    CalcZeroFlag( executeResult );\
    CalcSignFlag( executeResult );\
    uint8_t saveValue = ( uint8_t )executeResult;

#define Ora\
    uint32_t executeResult = ( uint16_t )m_Registers.AC | operandValue;\
    CalcZeroFlag( executeResult );\
    CalcSignFlag( executeResult );\
    m_Registers.AC = ( uint8_t ) executeResult;\
    if( true == isAdditionCycleEnable )\
        cycles++;
      
#define Rol\
    uint32_t executeResult = operandValue << 1;\
    executeResult |= m_Registers.SR.C;\
    CalcCarryFlag( executeResult );\
    CalcZeroFlag( executeResult );\
    CalcSignFlag( executeResult );\
    uint8_t saveValue = ( uint8_t )executeResult;
        
#define Bcc\
    if( m_Registers.SR.C == CLR_BIT )\
    {\
        CpuProgCounterReg_t oldpc;\
        oldpc.value = m_Registers.PC.value;\
        m_Registers.PC.value += operandAddr.value;\
        cycles++;\
        if ( oldpc.highPart != m_Registers.PC.highPart ) /*check if jump crossed a page boundary*/ \
            cycles++;\
    }
    
#define Bcs\
    if( m_Registers.SR.C == SET_BIT )\
    {\
        CpuProgCounterReg_t oldpc;\
        oldpc.value = m_Registers.PC.value;\
        m_Registers.PC.value += operandAddr.value;\
        cycles++;\
        if ( oldpc.highPart != m_Registers.PC.highPart ) /*check if jump crossed a page boundary*/ \
            cycles ++;\
    }
    
#define Beq\
    if ( m_Registers.SR.Z == SET_BIT )\
    {\
        CpuProgCounterReg_t oldpc;\
        oldpc.value = m_Registers.PC.value;\
        m_Registers.PC.value += operandAddr.value;\
        cycles++;\
        if( oldpc.highPart != m_Registers.PC.highPart ) /*check if jump crossed a page boundary*/ \
            cycles++;\
    }    
    
#define Bmi\
    if( m_Registers.SR.N == SET_BIT )\
    {\
        CpuProgCounterReg_t oldpc;\
        oldpc.value = m_Registers.PC.value;\
        m_Registers.PC.value += operandAddr.value;\
        cycles++;\
        if( oldpc.highPart != m_Registers.PC.highPart ) /*check if jump crossed a page boundary*/ \
            cycles++;\
    }
    
#define Bne\
    if( m_Registers.SR.Z == CLR_BIT )\
    {\
        CpuProgCounterReg_t oldpc;\
        oldpc.value = m_Registers.PC.value;\
        m_Registers.PC.value += operandAddr.value;\
        cycles++;\
        if ( oldpc.highPart != m_Registers.PC.highPart ) /*check if jump crossed a page boundary*/ \
            cycles++;\
    }
    
#define Bpl\
    if( m_Registers.SR.N == CLR_BIT )\
    {\
        CpuProgCounterReg_t oldpc;\
        oldpc.value = m_Registers.PC.value;\
        m_Registers.PC.value += operandAddr.value;\
        cycles++;\
        if ( oldpc.highPart != m_Registers.PC.highPart ) /*check if jump crossed a page boundary*/ \
            cycles ++;\
    }
      
#define Bvc\
    if( m_Registers.SR.V == CLR_BIT )\
    {\
        CpuProgCounterReg_t oldpc;\
        oldpc.value = m_Registers.PC.value;\
        m_Registers.PC.value += operandAddr.value;\
        cycles++;\
        if ( oldpc.highPart != m_Registers.PC.highPart ) /*check if jump crossed a page boundary*/ \
            cycles ++;\
    }
    
#define Bvs\
    if( m_Registers.SR.V == SET_BIT ) \
    {\
        CpuProgCounterReg_t oldpc;\
        oldpc.value = m_Registers.PC.value;\
        m_Registers.PC.value += operandAddr.value;\
        cycles++;\
        if ( oldpc.highPart != m_Registers.PC.highPart ) /*check if jump crossed a page boundary*/ \
            cycles++;\
    }    
    
 
#define Adc\
    uint32_t executeResult = ( uint16_t )m_Registers.AC + operandValue + ( uint16_t )m_Registers.SR.C;\
    CalcCarryFlag( executeResult );\
    CalcZeroFlag( executeResult );\
    CalcOverflowFlag( executeResult, m_Registers.AC, operandValue );\
    CalcSignFlag( executeResult );\
    m_Registers.AC = ( uint8_t )executeResult;\
    if( true == isAdditionCycleEnable )\
        cycles++;
    
#define And\
    operandValue &= m_Registers.AC;\
    CalcZeroFlag( operandValue );\
    CalcSignFlag( operandValue );\
    m_Registers.AC = ( uint8_t )( operandValue & 0x00FF );\
    if( true == isAdditionCycleEnable )\
        cycles++;
        
#define Asl\
    operandValue <<= 1;\
    CalcCarryFlag( operandValue );\
    CalcZeroFlag( operandValue );\
    CalcSignFlag( operandValue );\
    uint8_t saveValue = ( uint8_t )operandValue;    
        
#define Bit\
    uint32_t executeResult = ( uint16_t )m_Registers.AC & operandValue;\
    CalcZeroFlag( executeResult );\
    CalcSignFlag( executeResult );\
    m_Registers.SR.value = ( m_Registers.SR.value & 0xBF ) | ( uint8_t )( operandValue & 0xC0 );

#define Cmp\
    uint32_t executeResult = ( uint16_t )m_Registers.AC - operandValue;\
    m_Registers.SR.C = (m_Registers.AC >= ( uint8_t )operandValue ) ? SET_BIT : CLR_BIT;\
    m_Registers.SR.Z = (m_Registers.AC == ( uint8_t )operandValue ) ? SET_BIT : CLR_BIT;\
    CalcSignFlag( executeResult );\
    if( true == isAdditionCycleEnable )\
        cycles++;
    
#define Cpx\
    uint32_t executeResult = ( uint16_t )m_Registers.X - operandValue;\
    m_Registers.SR.C = ( m_Registers.X >= ( uint8_t )operandValue ) ? SET_BIT : CLR_BIT;\
    m_Registers.SR.Z = ( m_Registers.X == ( uint8_t )operandValue ) ? SET_BIT : CLR_BIT;\
    CalcSignFlag( executeResult );
    
#define Cpy\
    uint32_t executeResult = ( uint16_t )m_Registers.Y - operandValue;\
    m_Registers.SR.C = ( m_Registers.Y >= ( uint8_t )operandValue ) ? SET_BIT : CLR_BIT;\
    m_Registers.SR.Z= (m_Registers.Y == ( uint8_t )operandValue ) ? SET_BIT : CLR_BIT;\
    CalcSignFlag( executeResult );
    
#define Dec\
    operandValue--;\
    CalcZeroFlag( operandValue );\
    CalcSignFlag(operandValue);\
    uint8_t saveValue = ( uint8_t )operandValue;
    
#define Eor\
    uint32_t executeResult = ( uint16_t )m_Registers.AC ^ operandValue;\
    CalcZeroFlag( executeResult );\
    CalcSignFlag( executeResult );\
    m_Registers.AC = ( uint8_t )executeResult;\
    if( true == isAdditionCycleEnable )\
        cycles++;    
  
#define Jmp m_Registers.PC.value = operandAddr.value;
#define Sta uint8_t saveValue = m_Registers.AC    
#define Stx uint8_t saveValue = m_Registers.X;    
#define Sty uint8_t saveValue = m_Registers.Y;
#define Clc m_Registers.SR.C = CLR_BIT;  
#define Plp m_Registers.SR.value = StackPop(); 
#define Sec m_Registers.SR.C = SET_BIT; 
#define Pha StackPush( m_Registers.AC ); 
#define Cli m_Registers.SR.I = CLR_BIT; 
#define Txs m_Registers.SP = m_Registers.X; 
#define Clv m_Registers.SR.V = CLR_BIT; 
#define Cld m_Registers.SR.D = CLR_BIT; 
#define Sei m_Registers.SR.I = SET_BIT; 
#define Sed m_Registers.SR.D = SET_BIT; 
#define Php StackPush( m_Registers.SR.value );  //!!!          
        
        
// ------------------------------------------ //
#define Imm uint16_t operandValue  = fetchWord;\

#define Impl\
               
#define Acc uint16_t operandValue = m_Registers.AC;
#define Abs operandAddr.value = fetchWord;    

#define Rel\
    operandAddr.value = fetchWord;\
    if( operandAddr.value & 0x80 )\
        operandAddr.highPart = 0xFF;
      
#define Zp operandAddr.value = fetchWord;   
        
#define ZpX\
    operandAddr.value = fetchWord + m_Registers.X;\
    operandAddr.highPart = 0; /*zero-page wraparound*/
        
#define ZpY\
    operandAddr.value = fetchWord + m_Registers.Y;\
    operandAddr.highPart = 0;    //zero-page wraparound
    
#define AbsX {\
    operandAddr.value = fetchWord;\
    uint16_t startPageAddr = operandAddr.value;\
    operandAddr.value += m_Registers.X;\
    if( ( startPageAddr ^ operandAddr.value ) & 0x100 ) /*page-crossing*/ \
        isAdditionCycleEnable = true;}

#define AbsY {\
    operandAddr.value = fetchWord;\
    uint16_t startPageAddr = operandAddr.value;\
    operandAddr.value += m_Registers.Y; /*(target^tmp)&0x100*/ \
    if ( ( operandAddr.value ^ startPageAddr ) & 0x100 ) /*page-crossing*/ \
        isAdditionCycleEnable = true;}

#define Ind {\
    uint16_t tempAddr = (fetchWord & 0xFF00) | ((fetchWord + 1) & 0x00FF); /*replicate 6502 page-boundary wraparound bug*/ \
    operandAddr.lowPart  = BusRead( fetchWord);\
    operandAddr.highPart = BusRead( tempAddr);}

#define IndX {\
    uint16_t tempAddr =  ( uint16_t )( ( fetchWord + ( uint16_t )m_Registers.X) & 0xFF); /*zero-page wraparound for table pointer*/ \
    operandAddr.lowPart  = BusRead( tempAddr & 0x00FF);\
    operandAddr.highPart = BusRead( (tempAddr+1) & 0x00FF);}

#define IndY {\
    operandAddr.lowPart  = BusRead( fetchWord );\
    operandAddr.highPart = BusRead( fetchWord + 1 );\
    uint16_t startPageAddr = operandAddr.value;\
    operandAddr.value += m_Registers.Y;\
    if( ( operandAddr.value ^ startPageAddr ) & 0x100 ) /*page-crossing*/ \
    {\
        operandAddr.value &= 0xFFFF;\
        isAdditionCycleEnable = true;\
    }}
// ------------------------------------------ //
    
#define OpRead      uint16_t operandValue = BusRead( operandAddr.value );  
#define SaveRes     BusWrite( operandAddr.value, saveValue );
#define SaveResAcc  m_Registers.AC = saveValue;

#define FetchWord\
    uint16_t fetchWord = BusRead( m_Registers.PC.value++ );\
    fetchWord |= BusRead( m_Registers.PC.value++ ) << 8;
        
#define FetchByte\
    uint16_t fetchWord = BusRead( m_Registers.PC.value++ );

void Cpu_t::StackPush( uint8_t pushval ) 
{
    BusWrite( BASE_STACK_ADDR + m_Registers.SP, pushval );
    m_Registers.SP --;
}

uint8_t Cpu_t::StackPop() 
{
    m_Registers.SP ++;
    return BusRead( BASE_STACK_ADDR + m_Registers.SP ); 
}

void Cpu_t::Reset() 
{
    m_IsNonMaskableInterrupt = false;
    m_IsInterruptRequest     = false;
    m_IsReset                = true; 
}

void Cpu_t::NonMaskableInterrupt() 
{
    m_IsNonMaskableInterrupt = true;
}

void Cpu_t::InterruptRequest() 
{
    m_IsInterruptRequest = true;
}

uint32_t Cpu_t::ExecuteOneCommand()
{
    uint32_t cycles = 0;

    if( true == m_IsReset )
    {
        m_Registers.PC.lowPart  = BusRead( BEGIN_PC_LOW_PART_VALUE_BUS_ADDR );
        m_Registers.PC.highPart = BusRead( BEGIN_PC_HIGH_PART_VALUE_BUS_ADDR );
        m_Registers.AC          = 0;
        m_Registers.X           = 0;
        m_Registers.Y           = 0;
        m_Registers.SP          = 0xFF;
        m_Registers.SR.value    = 0;
        m_Registers.SR.I        = SET_BIT;
        m_IsReset = false;
    }
    else if( true == m_IsNonMaskableInterrupt )
    {
        cycles = 7;
        StackPush(m_Registers.PC.highPart);
        StackPush(m_Registers.PC.lowPart);
        StackPush(m_Registers.SR.value);
        m_Registers.SR.I = SET_BIT;
        m_Registers.PC.lowPart  = BusRead( 0xFFFA );
        m_Registers.PC.highPart = BusRead( 0xFFFB );
        m_IsNonMaskableInterrupt = false;
    }
    else if( true == m_IsInterruptRequest )
    {
        
        if( SET_BIT != m_Registers.SR.I ) 
        {
            cycles = 7;
            StackPush( m_Registers.PC.highPart );
            StackPush( m_Registers.PC.lowPart );
            StackPush( m_Registers.SR.value );
            m_Registers.SR.I = SET_BIT;
            m_Registers.PC.lowPart  = BusRead( 0xFFFE );
            m_Registers.PC.highPart = BusRead( 0xFFFF );
        }
        m_IsInterruptRequest = false;
    }
    
    uint8_t opcode = BusRead( m_Registers.PC.value++ );
         
    cycles += CycTable[ opcode ];
    
    CpuProgCounterReg_t operandAddr;
    
    operandAddr.value = 0;

    bool isAdditionCycleEnable = false;
    
    switch( opcode )
    {
        case ZP_ORA    : { FetchByte;   Zp; OpRead; Ora;          } break;
        case ZP_ASL    : { FetchByte;   Zp; OpRead; Asl; SaveRes; } break;
        case ZP_BIT    : { FetchByte;   Zp; OpRead; Bit;          } break;
        case ZP_AND    : { FetchByte;   Zp; OpRead; And;          } break;
        case ZP_ROL    : { FetchByte;   Zp; OpRead; Rol; SaveRes; } break;
        case ZP_EOR    : { FetchByte;   Zp; OpRead; Eor;          } break;
        case ZP_LSR    : { FetchByte;   Zp; OpRead; Lsr; SaveRes; } break;
        case ZP_ADC    : { FetchByte;   Zp; OpRead; Adc;          } break;
        case ZP_ROR    : { FetchByte;   Zp; OpRead; Ror; SaveRes; } break;
        case ZP_STY    : { FetchByte;   Zp;         Sty; SaveRes; } break;
        case ZP_STA    : { FetchByte;   Zp;         Sta; SaveRes; } break;
        case ZP_STX    : { FetchByte;   Zp;         Stx; SaveRes; } break;
        case ZP_LDY    : { FetchByte;   Zp; OpRead; Ldy;          } break;
        case ZP_LDA    : { FetchByte;   Zp; OpRead; Lda;          } break;
        case ZP_LDX    : { FetchByte;   Zp; OpRead; Ldx;          } break;
        case ZP_CPY    : { FetchByte;   Zp; OpRead; Cpy;          } break;
        case ZP_CMP    : { FetchByte;   Zp; OpRead; Cmp;          } break;
        case ZP_DEC    : { FetchByte;   Zp; OpRead; Dec; SaveRes; } break;
        case ZP_CPX    : { FetchByte;   Zp; OpRead; Cpx;          } break;
        case ZP_SBC    : { FetchByte;   Zp; OpRead; Sbc;          } break;
        case ZP_INC    : { FetchByte;   Zp; OpRead; Inc; SaveRes; } break;

        case ABS_ORA   : { FetchWord;  Abs; OpRead; Ora;          } break;
        case ABS_ASL   : { FetchWord;  Abs; OpRead; Asl; SaveRes; } break;
        case ABS_BIT   : { FetchWord;  Abs; OpRead; Bit;          } break;
        case ABS_AND   : { FetchWord;  Abs; OpRead; And;          } break;
        case ABS_ROL   : { FetchWord;  Abs; OpRead; Rol; SaveRes; } break;
        case ABS_JSR   : { FetchWord;  Abs;         Jsr;          } break;    
        case ABS_JMP   : { FetchWord;  Abs;         Jmp;          } break;
        case ABS_EOR   : { FetchWord;  Abs; OpRead; Eor;          } break;
        case ABS_LSR   : { FetchWord;  Abs; OpRead; Lsr; SaveRes; } break;  
        case ABS_ADC   : { FetchWord;  Abs; OpRead; Adc;          } break;
        case ABS_ROR   : { FetchWord;  Abs; OpRead; Ror; SaveRes; } break;    
        case ABS_STY   : { FetchWord;  Abs;         Sty; SaveRes; } break;
        case ABS_STA   : { FetchWord;  Abs;         Sta; SaveRes; } break;
        case ABS_STX   : { FetchWord;  Abs;         Stx; SaveRes; } break;
        case ABS_LDY   : { FetchWord;  Abs; OpRead; Ldy;          } break;
        case ABS_LDA   : { FetchWord;  Abs; OpRead; Lda;          } break;
        case ABS_LDX   : { FetchWord;  Abs; OpRead; Ldx;          } break;
        case ABS_CPY   : { FetchWord;  Abs; OpRead; Cpy;          } break;
        case ABS_CMP   : { FetchWord;  Abs; OpRead; Cmp;          } break;
        case ABS_DEC   : { FetchWord;  Abs; OpRead; Dec; SaveRes; } break;    
        case ABS_CPX   : { FetchWord;  Abs; OpRead; Cpx;          } break;
        case ABS_SBC   : { FetchWord;  Abs; OpRead; Sbc;          } break;
        case ABS_INC   : { FetchWord;  Abs; OpRead; Inc; SaveRes; } break;

        case ABS_X_ORA : { FetchWord; AbsX; OpRead; Ora;          } break;
        case ABS_X_ASL : { FetchWord; AbsX; OpRead; Asl; SaveRes; } break;
        case ABS_X_AND : { FetchWord; AbsX; OpRead; And;          } break;
        case ABS_X_ROL : { FetchWord; AbsX; OpRead; Rol; SaveRes; } break;
        case ABS_X_EOR : { FetchWord; AbsX; OpRead; Eor;          } break;
        case ABS_X_LSR : { FetchWord; AbsX; OpRead; Lsr; SaveRes; } break;
        case ABS_X_ADC : { FetchWord; AbsX; OpRead; Adc;          } break;
        case ABS_X_ROR : { FetchWord; AbsX; OpRead; Ror; SaveRes; } break;
        case ABS_X_STA : { FetchWord; AbsX;         Sta; SaveRes; } break;
        case ABS_X_LDY : { FetchWord; AbsX; OpRead; Ldy;          } break;
        case ABS_X_LDA : { FetchWord; AbsX; OpRead; Lda;          } break;
        case ABS_X_CMP : { FetchWord; AbsX; OpRead; Cmp;          } break;
        case ABS_X_DEC : { FetchWord; AbsX; OpRead; Dec; SaveRes; } break;
        case ABS_X_SBC : { FetchWord; AbsX; OpRead; Sbc;          } break;
        case ABS_X_INC : { FetchWord; AbsX; OpRead; Inc; SaveRes; } break;

        case ZP_X_ORA  : { FetchByte;  ZpX; OpRead; Ora;          } break;
        case ZP_X_ASL  : { FetchByte;  ZpX; OpRead; Asl; SaveRes; } break;
        case ZP_X_AND  : { FetchByte;  ZpX; OpRead; And;          } break;
        case ZP_X_ROL  : { FetchByte;  ZpX; OpRead; Rol; SaveRes; } break;
        case ZP_X_EOR  : { FetchByte;  ZpX; OpRead; Eor;          } break;
        case ZP_X_LSR  : { FetchByte;  ZpX; OpRead; Lsr; SaveRes; } break;
        case ZP_X_ADC  : { FetchByte;  ZpX; OpRead; Adc;          } break;
        case ZP_X_ROR  : { FetchByte;  ZpX; OpRead; Ror; SaveRes; } break;
        case ZP_X_STY  : { FetchByte;  ZpX;         Sty; SaveRes; } break;
        case ZP_X_STA  : { FetchByte;  ZpX;         Sta; SaveRes; } break;
        case ZP_X_LDY  : { FetchByte;  ZpX; OpRead; Ldy;          } break;
        case ZP_X_LDA  : { FetchByte;  ZpX; OpRead; Lda;          } break;    
        case ZP_X_CMP  : { FetchByte;  ZpX; OpRead; Cmp;          } break;
        case ZP_X_DEC  : { FetchByte;  ZpX; OpRead; Dec; SaveRes; } break;
        case ZP_X_SBC  : { FetchByte;  ZpX; OpRead; Sbc;          } break;
        case ZP_X_INC  : { FetchByte;  ZpX; OpRead; Inc; SaveRes; } break;

        case ABS_Y_ORA : { FetchWord; AbsY; OpRead; Ora;          } break;
        case ABS_Y_AND : { FetchWord; AbsY; OpRead; And;          } break;
        case ABS_Y_EOR : { FetchWord; AbsY; OpRead; Eor;          } break;
        case ABS_Y_ADC : { FetchWord; AbsY; OpRead; Adc;          } break;
        case ABS_Y_STA : { FetchWord; AbsY;         Sta; SaveRes; } break;
        case ABS_Y_LDA : { FetchWord; AbsY; OpRead; Lda;          } break;
        case ABS_Y_LDX : { FetchWord; AbsY; OpRead; Ldx;          } break;
        case ABS_Y_CMP : { FetchWord; AbsY; OpRead; Cmp;          } break;
        case ABS_Y_SBC : { FetchWord; AbsY; OpRead; Sbc;          } break;

        case IND_Y_ORA : { FetchByte; IndY; OpRead; Ora;          } break;
        case IND_Y_AND : { FetchByte; IndY; OpRead; And;          } break;
        case IND_Y_EOR : { FetchByte; IndY; OpRead; Eor;          } break;
        case IND_Y_ADC : { FetchByte; IndY; OpRead; Adc;          } break;
        case IND_Y_STA : { FetchByte; IndY;         Sta; SaveRes; } break;
        case IND_Y_LDA : { FetchByte; IndY; OpRead; Lda;          } break;
        case IND_Y_CMP : { FetchByte; IndY; OpRead; Cmp;          } break;
        case IND_Y_SBC : { FetchByte; IndY; OpRead; Sbc;          } break;

        case IND_X_ORA : { FetchByte; IndX; OpRead; Ora;          } break;
        case IND_X_AND : { FetchByte; IndX; OpRead; And;          } break;
        case IND_X_EOR : { FetchByte; IndX; OpRead; Eor;          } break;
        case IND_X_ADC : { FetchByte; IndX; OpRead; Adc;          } break;
        case IND_X_STA : { FetchByte; IndX;         Sta; SaveRes; } break;
        case IND_X_LDA : { FetchByte; IndX; OpRead; Lda;          } break;
        case IND_X_CMP : { FetchByte; IndX; OpRead; Cmp;          } break;
        case IND_X_SBC : { FetchByte; IndX; OpRead; Sbc;          } break;

        case ZP_Y_STX  : { FetchByte;  ZpY;         Stx; SaveRes; } break;
        case ZP_Y_LDX  : { FetchByte;  ZpY; OpRead; Ldx;          } break;

        case IND_JMP   : { FetchWord;  Ind;         Jmp;          } break;

        case IMM_ORA   : { FetchByte; Imm; Ora; } break;
        case IMM_AND   : { FetchByte; Imm; And; } break;
        case IMM_EOR   : { FetchByte; Imm; Eor; } break;    
        case IMM_ADC   : { FetchByte; Imm; Adc; } break;
        case IMM_LDY   : { FetchByte; Imm; Ldy; } break;
        case IMM_LDX   : { FetchByte; Imm; Ldx; } break;
        case IMM_LDA   : { FetchByte; Imm; Lda; } break;    
        case IMM_CPY   : { FetchByte; Imm; Cpy; } break;   
        case IMM_CPX   : { FetchByte; Imm; Cpx; } break;  
        case IMM_SBC   : { FetchByte; Imm; Sbc; } break;    
        case IMM_CMP   : { FetchByte; Imm; Cmp; } break;   

        case ACC_ASL   : { Acc; Asl; SaveResAcc; } break;
        case ACC_ROL   : { Acc; Rol; SaveResAcc; } break;    
        case ACC_LSR   : { Acc; Lsr; SaveResAcc; } break;
        case ACC_ROR   : { Acc; Ror; SaveResAcc; } break;    
     
        case REL_BPL   : { FetchByte; Rel; Bpl; } break;
        case REL_BMI   : { FetchByte; Rel; Bmi; } break;
        case REL_BVC   : { FetchByte; Rel; Bvc; } break;   
        case REL_BVS   : { FetchByte; Rel; Bvs; } break;
        case REL_BCC   : { FetchByte; Rel; Bcc; } break;
        case REL_BCS   : { FetchByte; Rel; Bcs; } break;
        case REL_BNE   : { FetchByte; Rel; Bne; } break;
        case REL_BEQ   : { FetchByte; Rel; Beq; } break;
                    
        case IMPL_BRK  : { Brk; } break;
        case IMPL_RTI  : { Rti; } break;
        case IMPL_RTS  : { Rts; } break;
        case IMPL_PLA  : { Pla; } break;
        case IMPL_DEY  : { Dey; } break;
        case IMPL_TXA  : { Txa; } break;
        case IMPL_TYA  : { Tya; } break;
        case IMPL_TAY  : { Tay; } break;
        case IMPL_TAX  : { Tax; } break;
        case IMPL_TSX  : { Tsx; } break;
        case IMPL_INY  : { Iny; } break;
        case IMPL_DEX  : { Dex; } break;
        case IMPL_INX  : { Inx; } break;
        case IMPL_NOP  : {      } break;
        case IMPL_CLC  : { Clc; } break; 
        case IMPL_PLP  : { Plp; } break;
        case IMPL_SEC  : { Sec; } break;
        case IMPL_PHA  : { Pha; } break;
        case IMPL_CLI  : { Cli; } break;
        case IMPL_TXS  : { Txs; } break;
        case IMPL_CLV  : { Clv; } break;
        case IMPL_CLD  : { Cld; } break;
        case IMPL_SEI  : { Sei; } break;
        case IMPL_SED  : { Sed; } break;
        case IMPL_PHP  : { Php; } break; //!!!

        default: break;
    };
   
    return cycles;
}
