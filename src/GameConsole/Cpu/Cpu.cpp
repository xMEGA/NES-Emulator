/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "Cpu.h"
#include "CpuRegisters.h"
#include "CpuOpCodes.h"

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

uint32_t Cpu_t::DumpMemory( uint8_t* pData, uint16_t startAddr, uint32_t bytesCnt )
{
    
    for( uint32_t idx = 0; idx < bytesCnt; idx++ )
    {
        pData[ idx ] = fp_BusReadCallBack( m_Context, startAddr + idx ); 
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

uint8_t Cpu_t::StackPop() 
{
    m_Registers.SP ++;
    return fp_BusReadCallBack( m_Context, BASE_STACK_ADDR + m_Registers.SP ); 
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

uint8_t Cpu_t::CommandExecute( ExecContext_t* pExecContext )
{
    CpuProgCounterReg_t oldpc;

    uint8_t saveValue = 0;
    
    uint16_t executeResult = 0;
    
    uint16_t operandValue = pExecContext->OperandValue;
    
    switch( pExecContext->Mnemonic )
    {
    case ADC:
        executeResult = (uint16_t)m_Registers.AC + operandValue + (uint16_t)m_Registers.SR.C;
        CalcCarryFlag( executeResult );
        CalcZeroFlag( executeResult );
        CalcOverflowFlag( executeResult, m_Registers.AC, operandValue );
        CalcSignFlag( executeResult );

        if( true == pExecContext->IsAdditionCycleEnable )
        {
            pExecContext->ExecuteCycles++;
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
            pExecContext->ExecuteCycles++;
        }
        #endif
   
        m_Registers.AC = (uint8_t)executeResult;

    break;


    case AND:
        operandValue &= m_Registers.AC;
        CalcZeroFlag( operandValue );
        CalcSignFlag( operandValue );
        m_Registers.AC = (uint8_t)( operandValue & 0x00FF );
        
        if( true == pExecContext->IsAdditionCycleEnable )
        {
            pExecContext->ExecuteCycles++;
        }
    break;

    case ASL:
        operandValue <<= 1;
        CalcCarryFlag( operandValue );
        CalcZeroFlag( operandValue );
        CalcSignFlag( operandValue );
        saveValue = (uint8_t)operandValue;
    break;

    case BIT:
        executeResult = (uint16_t)m_Registers.AC & operandValue;
        CalcZeroFlag( executeResult );
        CalcSignFlag( executeResult );
        m_Registers.SR.value = (m_Registers.SR.value & 0xBF) | (uint8_t)(operandValue & 0xC0);
    break;

    case CMP:  
        executeResult = (uint16_t)m_Registers.AC - operandValue;
        if (m_Registers.AC >= (uint8_t)operandValue ) 
            m_Registers.SR.C = SET_BIT;
        else 
            m_Registers.SR.C =  CLR_BIT;
    
        if (m_Registers.AC == (uint8_t)operandValue ) 
            m_Registers.SR.Z= SET_BIT;
        else 
            m_Registers.SR.Z= CLR_BIT;
        CalcSignFlag( executeResult );
        
        if( true == pExecContext->IsAdditionCycleEnable )
        {
            pExecContext->ExecuteCycles++;
        }
    break;

    case CPX:
        executeResult = (uint16_t)m_Registers.X - operandValue;
   
        if ( m_Registers.X >= (uint8_t)operandValue ) 
            m_Registers.SR.C = SET_BIT;
        else
            m_Registers.SR.C =  CLR_BIT;
        if ( m_Registers.X == (uint8_t)operandValue ) 
            m_Registers.SR.Z= SET_BIT;
        else 
            m_Registers.SR.Z= CLR_BIT;
        CalcSignFlag( executeResult );
    break;

    case CPY: 
        executeResult = (uint16_t)m_Registers.Y - operandValue;
   
        if ( m_Registers.Y >= (uint8_t)operandValue )
            m_Registers.SR.C = SET_BIT;
        else 
            m_Registers.SR.C =  CLR_BIT;

        if (m_Registers.Y == (uint8_t)operandValue ) 
            m_Registers.SR.Z= SET_BIT;
        else 
            m_Registers.SR.Z= CLR_BIT;
        CalcSignFlag( executeResult );
    break;

    case DEC: 
        operandValue --;
        CalcZeroFlag( operandValue );
        CalcSignFlag(operandValue);
        saveValue = (uint8_t)operandValue;
    break;

    case EOR: 
        executeResult = (uint16_t)m_Registers.AC ^ operandValue;   
        CalcZeroFlag( executeResult );
        CalcSignFlag( executeResult );
        m_Registers.AC = (uint8_t)executeResult;
        
        if( true == pExecContext->IsAdditionCycleEnable )
        {
            pExecContext->ExecuteCycles++;
        }
    break;

    case INC: 
        operandValue ++;
        CalcZeroFlag( operandValue );
        CalcSignFlag( operandValue );
        saveValue = (uint8_t)operandValue;
    break;

    case LDA:    
        m_Registers.AC = (uint8_t)operandValue; 
        CalcZeroFlag( m_Registers.AC );
        CalcSignFlag( m_Registers.AC );
        
        if( true == pExecContext->IsAdditionCycleEnable )
        {
            pExecContext->ExecuteCycles++;
        }
    break;

    case LDX:   
        m_Registers.X = (uint8_t)operandValue;
        CalcZeroFlag( m_Registers.X );
        CalcSignFlag( m_Registers.X );
       /* if( m_AdditionCycleEnable )
        {
            pExecContext->ExecuteCycles++;
        }*/
    break;

    case LDY:
        m_Registers.Y = (uint8_t)operandValue;
        CalcZeroFlag( m_Registers.Y );
        CalcSignFlag( m_Registers.Y );
        
        if( true == pExecContext->IsAdditionCycleEnable )
        {
            pExecContext->ExecuteCycles++;
        }
        
    break;

    case LSR: 
        executeResult = operandValue >> 1;
        if (operandValue & 1) 
            m_Registers.SR.C = SET_BIT;
        else
            m_Registers.SR.C =  CLR_BIT;
        CalcZeroFlag( executeResult );
        CalcSignFlag( executeResult ); 
        saveValue = (uint8_t)executeResult;
    break;

    case ORA:
        executeResult = (uint16_t)m_Registers.AC | operandValue;
        CalcZeroFlag( executeResult );
        CalcSignFlag( executeResult );
        m_Registers.AC = (uint8_t) executeResult;
        
        if( true == pExecContext->IsAdditionCycleEnable )
        {
            pExecContext->ExecuteCycles++;
        }
    break;

    case ROL:
        executeResult  = operandValue << 1;
        executeResult |= m_Registers.SR.C;
        CalcCarryFlag( executeResult );
        CalcZeroFlag( executeResult );
        CalcSignFlag( executeResult );
        saveValue = (uint8_t)executeResult;
    break;

    case BCC:
        if( m_Registers.SR.C == CLR_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += pExecContext->OperandAddr;
            
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
            {
                pExecContext->ExecuteCycles += 2;
            } 
            else
            {    
                pExecContext->ExecuteCycles++;
            }
        }
    break;

    case BCS:
        if( m_Registers.SR.C == SET_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += pExecContext->OperandAddr;
            
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
            {
                pExecContext->ExecuteCycles += 2; 
            } 
            else
            {
                pExecContext->ExecuteCycles++;
            }
        }
    break;

    case BEQ:
        if ( m_Registers.SR.Z == SET_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += pExecContext->OperandAddr;
            
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
                pExecContext->ExecuteCycles += 2; 
            else 
                pExecContext->ExecuteCycles++;
        }
    break;

    case BMI:
        if ( m_Registers.SR.N == SET_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += pExecContext->OperandAddr;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
                pExecContext->ExecuteCycles += 2; 
            else 
                pExecContext->ExecuteCycles++;
        }
    break;

    case BNE:
        if ( m_Registers.SR.Z == CLR_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += pExecContext->OperandAddr;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
                pExecContext->ExecuteCycles += 2; 
            else 
                pExecContext->ExecuteCycles++;
        }
    break;

    case BPL:
        if ( m_Registers.SR.N == CLR_BIT )
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += pExecContext->OperandAddr;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
                pExecContext->ExecuteCycles += 2;
            else 
                pExecContext->ExecuteCycles++;
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
            m_Registers.PC.value += pExecContext->OperandAddr;
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
            {
                pExecContext->ExecuteCycles += 2; 
            } 
            else
            {
                pExecContext->ExecuteCycles++;
            }
        }
    break;

    case BVS:
        if ( m_Registers.SR.V == SET_BIT ) 
        {
            oldpc.value = m_Registers.PC.value;
            m_Registers.PC.value += pExecContext->OperandAddr;
            
            if ( oldpc.highPart != m_Registers.PC.highPart ) //check if jump crossed a page boundary
            {
                pExecContext->ExecuteCycles += 2; 
            } 
            else
            {
                pExecContext->ExecuteCycles++;
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
        m_Registers.PC.value = pExecContext->OperandAddr;
    break;

    case JSR: 
        m_Registers.PC.value--;
        StackPush( m_Registers.PC.highPart );
        StackPush( m_Registers.PC.lowPart );
        m_Registers.PC.value = pExecContext->OperandAddr;
    break;
        
    case ROR:
        executeResult = operandValue >> 1;
        executeResult |= m_Registers.SR.C << 7; //!!!
   
        if ( operandValue & 0x01 ) 
            m_Registers.SR.C = SET_BIT;
        else 
            m_Registers.SR.C = CLR_BIT;
    
        CalcZeroFlag( executeResult );
        CalcSignFlag( executeResult );
        saveValue = ( uint8_t )executeResult;
    break;

    case SBC:
        operandValue ^= 0x00FF;
        executeResult = (uint16_t)m_Registers.AC + operandValue + (uint16_t)m_Registers.SR.C;
   
        CalcCarryFlag( executeResult );
        CalcZeroFlag( executeResult );
        CalcOverflowFlag( executeResult, m_Registers.AC, operandValue );
        CalcSignFlag( executeResult );

        if( true == pExecContext->IsAdditionCycleEnable )
        {
            pExecContext->ExecuteCycles++;
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
        
            pExecContext->ExecuteCycles++;
        }
        #endif
   
        m_Registers.AC = (uint8_t)executeResult;
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
                pExecContext->ExecuteCycles++;
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
        operandValue = StackPop();
        operandValue |= StackPop() << 8;
        m_Registers.PC.value = operandValue + 1;
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
        saveValue = m_Registers.AC;
    break;

    case STX:
        saveValue = m_Registers.X;
    break;

    case STY: 
        saveValue = m_Registers.Y;
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
    
    return saveValue;
}

uint8_t Cpu_t::CommandDecode( ExecContext_t* pExecContext )
{
      // m_Registers.SR.Res = SET_BIT;

    uint8_t opcode    = fp_BusReadCallBack( m_Context, m_Registers.PC.value );
    m_Registers.PC.value++;

    CpuDecodeTable_t decodeInfo = cpuDecodeTable[ opcode ];
    
    pExecContext->ExecuteCycles += ( uint8_t )decodeInfo.opCycles;

    uint16_t fetchWord = 0;
    
    uint8_t* pFetchWord = ( uint8_t* )&fetchWord;
            
    while( --decodeInfo.opBytes )
    {
        *pFetchWord = fp_BusReadCallBack( m_Context, m_Registers.PC.value );
        m_Registers.PC.value++;
        pFetchWord ++;
    }


    CpuProgCounterReg_t operandAddr;
    operandAddr.value = 0;
        
    pExecContext->OperandValue = 0;
    pExecContext->IsAdditionCycleEnable = false;

   
    switch( decodeInfo.addrMode )
    {
        case IMM_ADDR_MODE:
            pExecContext->OperandValue  = fetchWord;
            decodeInfo.opRead = false;
        break;

        case IMPL_ADDR_MODE:
            decodeInfo.opRead = false;
        break;

        case ACC_ADDR_MODE:
            pExecContext->OperandValue    = m_Registers.AC;
            decodeInfo.opRead = false;
        break;
        
        case REL_ADDR_MODE:
        {
            operandAddr.value = fetchWord;
            if( operandAddr.value & 0x80 ) 
            {
                operandAddr.highPart = 0xFF;
            }
        }
        break;

        case ZP_ADDR_MODE:
            operandAddr.value = fetchWord;
        break;
            
        case ZP_X_ADDR_MODE:
            operandAddr.value = fetchWord + m_Registers.X;
            operandAddr.highPart = 0; //zero-page wraparound
        break;
        
        case ZP_Y_ADDR_MODE:
            operandAddr.value = fetchWord + m_Registers.Y; 
            operandAddr.highPart = 0;    //zero-page wraparound
        break;

        case ABS_ADDR_MODE:
            operandAddr.value = fetchWord;
        break;

        case ABS_X_ADDR_MODE:
        {
            operandAddr.value = fetchWord;
            uint16_t startPageAddr = operandAddr.value;
            operandAddr.value += m_Registers.X;
            if( ( startPageAddr ^ operandAddr.value ) & 0x100 ) //page-crossing
            { 
                pExecContext->IsAdditionCycleEnable = true;
            }
        }
        break;

        case ABS_Y_ADDR_MODE:
        {
            operandAddr.value = fetchWord;
            uint16_t startPageAddr = operandAddr.value;
            operandAddr.value += m_Registers.Y; //(target^tmp)&0x100
            if ( ( operandAddr.value ^ startPageAddr ) & 0x100 ) //page-crossing
            { 
                pExecContext->IsAdditionCycleEnable = true;
            }
        }
        break;

        case IND_ADDR_MODE:
        {
            uint16_t tempAddr = (fetchWord & 0xFF00) | ((fetchWord + 1) & 0x00FF); //replicate 6502 page-boundary wraparound bug
            operandAddr.lowPart  = fp_BusReadCallBack( m_Context, fetchWord);
            operandAddr.highPart = fp_BusReadCallBack( m_Context, tempAddr);
        }
        break;
            
        case IND_X_ADDR_MODE:
        {
            uint16_t tempAddr =  (uint16_t)( ( fetchWord + (uint16_t)m_Registers.X) & 0xFF); //zero-page wraparound for table pointer
            operandAddr.lowPart  = fp_BusReadCallBack( m_Context, tempAddr & 0x00FF);
            operandAddr.highPart = fp_BusReadCallBack( m_Context, (tempAddr+1) & 0x00FF);
        }
        break;
            
        case IND_Y_ADDR_MODE:
        {
            //!!!uint16_t tempAddr = ( fetchWord & 0xFF00) | ((fetchWord + 1) & 0x00FF); //zero-page wraparound
    
            operandAddr.lowPart  = fp_BusReadCallBack( m_Context,  fetchWord );
            operandAddr.highPart = fp_BusReadCallBack( m_Context,  fetchWord + 1 );

            uint16_t startPageAddr = operandAddr.value;
            operandAddr.value += m_Registers.Y;
                  
            if( ( operandAddr.value ^ startPageAddr ) & 0x100 ) //page-crossing
            {
                operandAddr.value &= 0xFFFF;
                pExecContext->IsAdditionCycleEnable = true;
            }
        }
        break;
        
        default:
        break;
    }
        
    if( true == decodeInfo.opRead )
    {
        pExecContext->OperandValue = fp_BusReadCallBack( m_Context, operandAddr.value );
    }
   
    pExecContext->Mnemonic      = decodeInfo.mnemonic;
    pExecContext->OperandAddr   = operandAddr.value;
    
    return opcode;
}


uint8_t Cpu_t::ExecuteOneCommand()
{
    uint8_t executedCycles = 0;

    if( true == m_IsReset )
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
    else if( true == m_IsNonMaskableInterrupt )
    {
        executedCycles += 7;
        StackPush(m_Registers.PC.highPart);
        StackPush(m_Registers.PC.lowPart);
        StackPush(m_Registers.SR.value);
        m_Registers.SR.I = SET_BIT;
        m_Registers.PC.lowPart  = fp_BusReadCallBack( m_Context, 0xFFFA );
        m_Registers.PC.highPart = fp_BusReadCallBack( m_Context, 0xFFFB );
        m_IsNonMaskableInterrupt = false;
    }
    else if( true == m_IsInterruptRequest )
    {
        
        if( SET_BIT != m_Registers.SR.I ) 
        {
            executedCycles += 7;
            StackPush( m_Registers.PC.highPart );
            StackPush( m_Registers.PC.lowPart );
            StackPush( m_Registers.SR.value );
            m_Registers.SR.I = SET_BIT;
            m_Registers.PC.lowPart  = fp_BusReadCallBack( m_Context, 0xFFFE );
            m_Registers.PC.highPart = fp_BusReadCallBack( m_Context, 0xFFFF );
        }
        m_IsInterruptRequest = false;
    }
      
    ExecContext_t execContext = { 0 };
        
    execContext.ExecuteCycles = executedCycles;
    
    uint8_t opcode = CommandDecode( &execContext );
    uint8_t executeResult = CommandExecute( &execContext );
        
    if( true == cpuDecodeTable[ opcode ].resWrite )
    {
        if( ACC_ADDR_MODE == cpuDecodeTable[ opcode ].addrMode )
            m_Registers.AC = executeResult;
        else 
            fp_BusWriteCallBack( m_Context, execContext.OperandAddr, executeResult );
    }

    return execContext.ExecuteCycles;
}