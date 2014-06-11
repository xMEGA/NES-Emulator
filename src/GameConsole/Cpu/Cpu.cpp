/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Cpu.h"
#include "CpuRegisters.h"
#include "CpuOpCodes.h"

void Cpu_t::SaveCpuContext( uint8_t* pOutData )
{
    uint8_t* pData = pOutData;

    *pData ++= m_Registers.PC.lowPart;
    *pData ++= m_Registers.PC.highPart;
    *pData ++= m_Registers.AC;
    *pData ++= m_Registers.SR.value;
    *pData ++= m_Registers.X;
    *pData ++= m_Registers.Y;
    *pData ++= m_Registers.SP;
}


void Cpu_t::LoadCpuContext( uint8_t* pInData )
{
    uint8_t* pData = pInData;

    m_Registers.PC.lowPart  = *pData++;
    m_Registers.PC.highPart = *pData++;
    m_Registers.AC          = *pData++;
    m_Registers.SR.value    = *pData++;
    m_Registers.X           = *pData++;
    m_Registers.Y           = *pData++;
    m_Registers.SP          = *pData++;
}

void Cpu_t::SetBusWriteCallBack( CpuBusWriteCallBack_t busWriteCallBack , void * pContext )
{
    m_Context = pContext;
    fp_BusWriteCallBack = busWriteCallBack;
}

void Cpu_t::SetBusReadCallBack( CpuBusReadCallBack_t busReadCallBack , void * pContext )
{
    m_Context = pContext;
    fp_BusReadCallBack = busReadCallBack;
}


void Cpu_t::CalcZeroFlag( uint16_t n ) 
{
    if ( n & 0x00FF ) 
        m_Registers.SR.Z = CLR_BIT;
    else 
        m_Registers.SR.Z = SET_BIT;
}

void Cpu_t::CalcCarryFlag( uint16_t n ) 
{
    if ( n & 0xFF00 ) 
        m_Registers.SR.C = SET_BIT;
    else 
        m_Registers.SR.C = CLR_BIT;
}

void Cpu_t::CalcSignFlag( uint16_t n) 
{
    if ( n & 0x0080 ) 
        m_Registers.SR.N = SET_BIT;
    else 
        m_Registers.SR.N = CLR_BIT;
}

void Cpu_t::CalcOverflowFlag( uint16_t execRes, uint16_t  acc, uint16_t  mem ) 
{
    if ( ( execRes ^ acc ) & ( execRes ^ mem ) & 0x0080 ) 
        m_Registers.SR.V = SET_BIT;
    else 
        m_Registers.SR.V = CLR_BIT;
}

void Cpu_t::StackPush(uint8_t pushval) 
{
    fp_BusWriteCallBack( m_Context, BASE_STACK_ADDR + m_Registers.SP, pushval );
    m_Registers.SP --;
}

uint8_t Cpu_t::StackPop( void ) 
{
    m_Registers.SP ++;
    return fp_BusReadCallBack( m_Context, BASE_STACK_ADDR + m_Registers.SP ); 
}

void Cpu_t::Reset( void ) 
{
    m_IsNonMaskableInterrupt = false;
    m_IsInterruptRequest     = false;
    m_IsReset                = true; 
}

void Cpu_t::NonMaskableInterrupt( void ) 
{
    m_IsNonMaskableInterrupt = true;
}

void Cpu_t::InterruptRequest( void) 
{
    m_IsInterruptRequest = true;
}

void Cpu_t::AddressCalc( void )
{
    uint16_t tempAddr;
    uint16_t startPageAddr;
    
    switch( m_CpuDecodeInfo.addrMode )
    {
        case IMM_ADDR_MODE:
            m_OperandValue  = m_FetchWord;
            m_OpValReadNeed = false;
        break;

        case IMPL_ADDR_MODE:
            m_OpValReadNeed = false;
        break;

        case ACC_ADDR_MODE:
            m_OperandValue    = m_Registers.AC;
            m_OpValReadNeed = false;
        break;
        
        case REL_ADDR_MODE:
            m_OperandAddr.value = m_FetchWord;
            if (m_OperandAddr.value & 0x80) 
            {
                m_OperandAddr.highPart = 0xFF;
            }
        break;

        case ZP_ADDR_MODE:
            m_OperandAddr.value = m_FetchWord;
        break;
            
        case ZP_X_ADDR_MODE:
            m_OperandAddr.value = m_FetchWord + m_Registers.X;
            m_OperandAddr.highPart = 0; //zero-page wraparound
        break;
        
        case ZP_Y_ADDR_MODE:
            m_OperandAddr.value = m_FetchWord + m_Registers.Y; 
            m_OperandAddr.highPart = 0;    //zero-page wraparound
        break;

        case ABS_ADDR_MODE:
            m_OperandAddr.value = m_FetchWord;
        break;

        case ABS_X_ADDR_MODE:
            m_OperandAddr.value = m_FetchWord;
            startPageAddr = m_OperandAddr.value;
            m_OperandAddr.value += m_Registers.X;
            if( ( startPageAddr ^ m_OperandAddr.value ) & 0x100 ) //page-crossing
            { 
                m_AdditionCycleEnable = true;
            }
        break;

        case ABS_Y_ADDR_MODE:
            m_OperandAddr.value = m_FetchWord;
            startPageAddr = m_OperandAddr.value;
            m_OperandAddr.value += m_Registers.Y; //(target^tmp)&0x100
            if ( ( m_OperandAddr.value ^ startPageAddr ) & 0x100 ) //page-crossing
            { 
                m_AdditionCycleEnable = true;
            }
        break;
            
        case IND_ADDR_MODE:
            tempAddr = (m_FetchWord & 0xFF00) | ((m_FetchWord + 1) & 0x00FF); //replicate 6502 page-boundary wraparound bug
            m_OperandAddr.lowPart  = fp_BusReadCallBack( m_Context, m_FetchWord);
            m_OperandAddr.highPart = fp_BusReadCallBack( m_Context, tempAddr);
        break;
            
        case IND_X_ADDR_MODE:
            tempAddr =  (uint16_t)( ( m_FetchWord + (uint16_t)m_Registers.X) & 0xFF); //zero-page wraparound for table pointer
            m_OperandAddr.lowPart  = fp_BusReadCallBack( m_Context, tempAddr & 0x00FF);
            m_OperandAddr.highPart = fp_BusReadCallBack( m_Context, (tempAddr+1) & 0x00FF);
        break;
            
        case IND_Y_ADDR_MODE:
            tempAddr = ( m_FetchWord & 0xFF00) | ((m_FetchWord + 1) & 0x00FF); //zero-page wraparound
    
            m_OperandAddr.lowPart  = fp_BusReadCallBack( m_Context,  m_FetchWord );
            m_OperandAddr.highPart = fp_BusReadCallBack( m_Context,  m_FetchWord + 1 );

            startPageAddr = m_OperandAddr.value;
            m_OperandAddr.value += m_Registers.Y;
                  
            if( ( m_OperandAddr.value ^ startPageAddr ) & 0x100 ) //page-crossing
            {
                m_OperandAddr.value&=0xFFFF;
                m_AdditionCycleEnable = true;
            }

        break;
        
        default:
        break;
    }
}

void Cpu_t::CommandExecute(void)
{
    CpuProgCounterReg_t oldpc;

    switch( m_CpuDecodeInfo.mnemonic )
    {
    case ADC:
        m_ExecuteResult = (uint16_t)m_Registers.AC + m_OperandValue + (uint16_t)m_Registers.SR.C;
        CalcCarryFlag( m_ExecuteResult );
        CalcZeroFlag( m_ExecuteResult );
        CalcOverflowFlag( m_ExecuteResult, m_Registers.AC, m_OperandValue );
        CalcSignFlag( m_ExecuteResult );

        if( m_AdditionCycleEnable )
        {
            m_Cycles++;
        }
    
        #ifdef CPU_BCD
        if (m_Registers.SR.value & FLAG_DECIMAL) 
        {
            m_Registers.SR.C =  CLR_BIT;
        
            if ((a & 0x0F) > 0x09) 
            {
                a += 0x06;
            }
            if ((a & 0xF0) > 0x90) 
            {
                a += 0x60;
                m_Registers.SR.C = SET_BIT;
            }
            m_Cycles++;
        }
        #endif
   
        m_Registers.AC = (uint8_t)m_ExecuteResult;

    break;


    case AND:
        m_OperandValue &= m_Registers.AC;
        CalcZeroFlag( m_OperandValue );
        CalcSignFlag( m_OperandValue );
        m_Registers.AC = (uint8_t)( m_OperandValue & 0x00FF );
        if( m_AdditionCycleEnable )
        {
            m_Cycles++;
        }
    break;

    case ASL:
        m_OperandValue <<= 1;
        CalcCarryFlag( m_OperandValue );
        CalcZeroFlag( m_OperandValue );
        CalcSignFlag( m_OperandValue );
        m_SaveValue = (uint8_t)m_OperandValue;
    break;

    case BIT:
        m_ExecuteResult = (uint16_t)m_Registers.AC & m_OperandValue;
        CalcZeroFlag( m_ExecuteResult );
        CalcSignFlag( m_ExecuteResult );
        m_Registers.SR.value = (m_Registers.SR.value & 0xBF) | (uint8_t)(m_OperandValue & 0xC0);
    break;

    case CMP:  
        m_ExecuteResult = (uint16_t)m_Registers.AC - m_OperandValue;
        if (m_Registers.AC >= (uint8_t)m_OperandValue ) 
            m_Registers.SR.C = SET_BIT;
        else 
            m_Registers.SR.C =  CLR_BIT;
    
        if (m_Registers.AC == (uint8_t)m_OperandValue ) 
            m_Registers.SR.Z= SET_BIT;
        else 
            m_Registers.SR.Z= CLR_BIT;
        CalcSignFlag( m_ExecuteResult );
        if( m_AdditionCycleEnable )
        {
            m_Cycles++;
        }
    break;

    case CPX:
        m_ExecuteResult = (uint16_t)m_Registers.X - m_OperandValue;
   
        if ( m_Registers.X >= (uint8_t)m_OperandValue ) 
            m_Registers.SR.C = SET_BIT;
        else
            m_Registers.SR.C =  CLR_BIT;
        if ( m_Registers.X == (uint8_t)m_OperandValue ) 
            m_Registers.SR.Z= SET_BIT;
        else 
            m_Registers.SR.Z= CLR_BIT;
        CalcSignFlag( m_ExecuteResult );
    break;

    case CPY: 
        m_ExecuteResult = (uint16_t)m_Registers.Y - m_OperandValue;
   
        if ( m_Registers.Y >= (uint8_t)m_OperandValue )
            m_Registers.SR.C = SET_BIT;
        else 
            m_Registers.SR.C =  CLR_BIT;

        if (m_Registers.Y == (uint8_t)m_OperandValue ) 
            m_Registers.SR.Z= SET_BIT;
        else 
            m_Registers.SR.Z= CLR_BIT;
        CalcSignFlag( m_ExecuteResult );
    break;

    case DEC: 
        m_OperandValue --;
        CalcZeroFlag( m_OperandValue );
        CalcSignFlag(m_OperandValue);
        m_SaveValue = (uint8_t)m_OperandValue;
    break;

    case EOR: 
        m_ExecuteResult = (uint16_t)m_Registers.AC ^ m_OperandValue;   
        CalcZeroFlag( m_ExecuteResult );
        CalcSignFlag( m_ExecuteResult );
        m_Registers.AC = (uint8_t)m_ExecuteResult;
        if( m_AdditionCycleEnable )
        {
            m_Cycles++;
        }
    break;

    case INC: 
        m_OperandValue ++;
        CalcZeroFlag( m_OperandValue );
        CalcSignFlag( m_OperandValue );
        m_SaveValue = (uint8_t)m_OperandValue;
    break;

    case LDA:    
        m_Registers.AC = (uint8_t)m_OperandValue; 
        CalcZeroFlag( m_Registers.AC );
        CalcSignFlag( m_Registers.AC );
        if( m_AdditionCycleEnable )
        {
            m_Cycles++;
        }
    break;

    case LDX:   
        m_Registers.X = (uint8_t)m_OperandValue;
        CalcZeroFlag( m_Registers.X );
        CalcSignFlag( m_Registers.X );
       /* if( m_AdditionCycleEnable )
        {
            m_Cycles++;
        }*/
    break;

    case LDY:
        m_Registers.Y = (uint8_t)m_OperandValue;
        CalcZeroFlag( m_Registers.Y );
        CalcSignFlag( m_Registers.Y );
        if( m_AdditionCycleEnable )
        {
            m_Cycles++;
        }
       break;

    case LSR: 
        m_ExecuteResult = m_OperandValue >> 1;
        if (m_OperandValue & 1) 
            m_Registers.SR.C = SET_BIT;
        else
            m_Registers.SR.C =  CLR_BIT;
        CalcZeroFlag( m_ExecuteResult );
        CalcSignFlag( m_ExecuteResult ); 
        m_SaveValue = (uint8_t)m_ExecuteResult;
    break;

    case ORA:
        m_ExecuteResult = (uint16_t)m_Registers.AC | m_OperandValue;
        CalcZeroFlag( m_ExecuteResult );
        CalcSignFlag( m_ExecuteResult );
        m_Registers.AC = (uint8_t) m_ExecuteResult;
        if( m_AdditionCycleEnable )
        {
            m_Cycles++;
        }
    break;

    case ROL:
        m_ExecuteResult  = m_OperandValue << 1;
        m_ExecuteResult |= m_Registers.SR.C;
        CalcCarryFlag( m_ExecuteResult );
        CalcZeroFlag( m_ExecuteResult );
        CalcSignFlag( m_ExecuteResult );
        m_SaveValue = (uint8_t)m_ExecuteResult;
    break;

    case BCC:
        if( m_Registers.SR.C == CLR_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += m_OperandAddr.value;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
            {
                m_Cycles += 2;
            } 
            else
            {    
                m_Cycles++;
            }
        }
    break;

    case BCS:
        if( m_Registers.SR.C == SET_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += m_OperandAddr.value;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
            {
                m_Cycles += 2; 
            } 
            else
            {
                m_Cycles++;
            }
        }
    break;

    case BEQ:
        if ( m_Registers.SR.Z == SET_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += m_OperandAddr.value;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
                m_Cycles += 2; 
            else 
                m_Cycles++;
        }
    break;

    case BMI:
        if ( m_Registers.SR.N == SET_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += m_OperandAddr.value;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
                m_Cycles += 2; 
            else 
                m_Cycles++;
        }
    break;

    case BNE:
        if ( m_Registers.SR.Z == CLR_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += m_OperandAddr.value;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
                m_Cycles += 2; 
            else 
                m_Cycles++;
        }
    break;

    case BPL:
        if ( m_Registers.SR.N == CLR_BIT )
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += m_OperandAddr.value;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
                m_Cycles += 2;
            else 
                m_Cycles++;
        }
    break;

    case BRK: 
        m_Registers.PC.value++;
        StackPush( m_Registers.PC.highPart );
        StackPush( m_Registers.PC.lowPart );
        m_Registers.SR.B = SET_BIT;            
        StackPush( m_Registers.SR.value );    
        m_Registers.SR.I = SET_BIT;
        m_Registers.PC.lowPart   = fp_BusReadCallBack( m_Context, 0xFFFE);
        m_Registers.PC.highPart  = fp_BusReadCallBack( m_Context, 0xFFFF);
    break;

    case BVC:
        if ( m_Registers.SR.V == CLR_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += m_OperandAddr.value;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
            {
                m_Cycles += 2; 
            } 
            else
            {
                m_Cycles++;
            }
        }
    break;

    case BVS:
        if ( m_Registers.SR.V == SET_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += m_OperandAddr.value;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
            {
                m_Cycles += 2; 
            } 
            else
            {
                m_Cycles++;
            }
        }
    break;

    case CLC:
        m_Registers.SR.C =  CLR_BIT;
    break;

    case CLD: 
        m_Registers.SR.D = CLR_BIT;
    break;

    case CLI: 
         m_Registers.SR.I = CLR_BIT;
    break;

    case CLV: 
        m_Registers.SR.V =  CLR_BIT;
    break;
    
    case DEX: 
        m_Registers.X--;
        CalcZeroFlag( m_Registers.X );
        CalcSignFlag( m_Registers.X );
    break;

    case DEY: 
        m_Registers.Y--;
        CalcZeroFlag( m_Registers.Y );
        CalcSignFlag( m_Registers.Y );
    break;

    case INX:
        m_Registers.X++;
        CalcZeroFlag( m_Registers.X );
        CalcSignFlag( m_Registers.X );
    break;

    case INY: 
        m_Registers.Y++;
        CalcZeroFlag( m_Registers.Y );
        CalcSignFlag( m_Registers.Y );
    break;

    case JMP: 
        m_Registers.PC.value = m_OperandAddr.value;
    break;

    case JSR: 
        m_Registers.PC.value--;
        StackPush( m_Registers.PC.highPart );
        StackPush( m_Registers.PC.lowPart );
        m_Registers.PC.value = m_OperandAddr.value;
    break;
        
    case ROR:
        m_ExecuteResult = m_OperandValue >> 1;
        m_ExecuteResult |= m_Registers.SR.C << 7; //!!!
   
        if ( m_OperandValue & 0x01 ) 
            m_Registers.SR.C = SET_BIT;
        else 
            m_Registers.SR.C = CLR_BIT;
    
        CalcZeroFlag( m_ExecuteResult );
        CalcSignFlag( m_ExecuteResult );
           m_SaveValue = ( uint8_t )m_ExecuteResult;
    break;

    case SBC:
        m_OperandValue ^= 0x00FF;
        m_ExecuteResult = (uint16_t)m_Registers.AC + m_OperandValue + (uint16_t)m_Registers.SR.C;
   
        CalcCarryFlag( m_ExecuteResult );
        CalcZeroFlag( m_ExecuteResult );
        CalcOverflowFlag( m_ExecuteResult, m_Registers.AC, m_OperandValue );
        CalcSignFlag( m_ExecuteResult );

        if( m_AdditionCycleEnable )
        {
            m_Cycles++;
        }
   
        #ifdef CPU_BCD
        if ( m_Registers.SR.D ) 
        {
            m_Registers.SR.C =  CLR_BIT;
        
            a -= 0x66;
            if ((a & 0x0F) > 0x09) {
                a += 0x06;
            }
            if ((a & 0xF0) > 0x90) {
                a += 0x60;
                m_Registers.SR.C = SET_BIT;
            }
        
            m_Cycles++;
        }
        #endif
   
        m_Registers.AC = (uint8_t)m_ExecuteResult;
    break;

    case NOP:
    /*
    switch (opcode) 
    {
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
            if( m_AdditionCycleEnable )
            {
                m_Cycles++;
            }
   
            break;
    }
    */
    break;

    case PHA: 
        StackPush( m_Registers.AC );
    break;

    case PHP:
        StackPush( m_Registers.SR.value );
    break;

    case PLA:
        m_Registers.AC = StackPop();
        CalcZeroFlag( m_Registers.AC );
        CalcSignFlag( m_Registers.AC );
    break;

    case PLP: 
        m_Registers.SR.value = StackPop();
        //m_Registers.SR.Res   = SET_BIT;
    break;

    case RTI:
        m_Registers.SR.value    = StackPop();
        m_Registers.PC.lowPart  = StackPop();
        m_Registers.PC.highPart = StackPop();
    break;

    case RTS:
        m_OperandValue = StackPop();
        m_OperandValue |= StackPop() << 8;
        m_Registers.PC.value = m_OperandValue + 1;
    break;

    case SEC: 
        m_Registers.SR.C = SET_BIT;
    break;

    case SED: 
        m_Registers.SR.D = SET_BIT;
    break;

    case SEI: 
         m_Registers.SR.I = SET_BIT;
    break;

    case STA:
        m_SaveValue = m_Registers.AC;
    break;

    case STX:
        m_SaveValue = m_Registers.X;
    break;

    case STY: 
        m_SaveValue = m_Registers.Y;
    break;

    case TAX: 
        m_Registers.X = m_Registers.AC;
        CalcZeroFlag( m_Registers.X );
        CalcSignFlag( m_Registers.X );
    break;

    case TAY:
        m_Registers.Y = m_Registers.AC;  
        CalcZeroFlag( m_Registers.Y );
        CalcSignFlag( m_Registers.Y );
    break;

    case TSX:
        m_Registers.X = m_Registers.SP;
        CalcZeroFlag( m_Registers.X );
        CalcSignFlag( m_Registers.X );
    break;

    case TXA: 
        m_Registers.AC = m_Registers.X;
        CalcZeroFlag( m_Registers.AC );
        CalcSignFlag( m_Registers.AC );
    break;

    case TXS:
        m_Registers.SP = m_Registers.X;
    break;

    case TYA:
        m_Registers.AC = m_Registers.Y;
        CalcZeroFlag( m_Registers.AC );
        CalcSignFlag( m_Registers.AC );
    break;

    case LAX:
    case SAX:
    case DCP:     
    case ISB:
    case SLO:
    case RLA:
    case SRE:
//    case RRA:
        // Undocumented
    break;
    
    default:

    break;
    
    }
}

uint8_t Cpu_t::Run( void )
{
    uint8_t   opcode;
    uint8_t*  pFetchWord = ( uint8_t * )&m_FetchWord;

    m_Cycles = 0;

    if( m_IsReset )
    {
        m_Registers.PC.lowPart  = fp_BusReadCallBack( m_Context, BEGIN_PC_LOW_PART_VALUE_BUS_ADDR );
        m_Registers.PC.highPart = fp_BusReadCallBack( m_Context, BEGIN_PC_HIGH_PART_VALUE_BUS_ADDR );
        m_Registers.AC          = 0;
        m_Registers.X           = 0;
        m_Registers.Y           = 0;
        m_Registers.SP          = 0xFF;
        m_Registers.SR.value    = 0;
        m_Registers.SR.I        = SET_BIT;
        m_IsReset = false;
    }
    else if( m_IsNonMaskableInterrupt )
    {
        m_Cycles += 7;
        StackPush(m_Registers.PC.highPart);
        StackPush(m_Registers.PC.lowPart);
        StackPush(m_Registers.SR.value);
        m_Registers.SR.I = SET_BIT;
        m_Registers.PC.lowPart  = fp_BusReadCallBack( m_Context, 0xFFFA );
        m_Registers.PC.highPart = fp_BusReadCallBack( m_Context, 0xFFFB );
        m_IsNonMaskableInterrupt = false;
    }
    else if( m_IsInterruptRequest )
    {
        
        if ( m_Registers.SR.I != SET_BIT ) 
        {
            m_Cycles += 7;
            StackPush(m_Registers.PC.highPart);
            StackPush(m_Registers.PC.lowPart);
            StackPush(m_Registers.SR.value);
            m_Registers.SR.I = SET_BIT;
            m_Registers.PC.lowPart  = fp_BusReadCallBack( m_Context, 0xFFFE );
            m_Registers.PC.highPart = fp_BusReadCallBack( m_Context, 0xFFFF );
        }
        m_IsInterruptRequest = false;
    }

    // m_Registers.SR.Res = SET_BIT;
    m_AdditionCycleEnable = false;

    m_OperandAddr.value = 0;
    m_OperandValue = 0;
    m_FetchWord = 0;
    
    opcode    = fp_BusReadCallBack( m_Context,  m_Registers.PC.value );
    m_Registers.PC.value++;
    m_CpuDecodeInfo = cpuDecodeTable[ opcode ];
    
    m_OpValReadNeed = ( bool )m_CpuDecodeInfo.opRead;
    m_Cycles       += ( uint8_t )m_CpuDecodeInfo.opCycles;

    while( -- m_CpuDecodeInfo.opBytes )
    {
        *pFetchWord = fp_BusReadCallBack( m_Context,  m_Registers.PC.value );
        m_Registers.PC.value++;
        pFetchWord ++;
    }
    
    AddressCalc();    

    if( m_OpValReadNeed )
    {
        m_OperandValue = fp_BusReadCallBack( m_Context,  m_OperandAddr.value );
    }

    CommandExecute();
        
    if( m_CpuDecodeInfo.resWrite )
    {
        if ( ACC_ADDR_MODE == m_CpuDecodeInfo.addrMode ) 
            m_Registers.AC = m_SaveValue;
        else 
            fp_BusWriteCallBack( m_Context, m_OperandAddr.value, m_SaveValue);
    }

     return m_Cycles;
}