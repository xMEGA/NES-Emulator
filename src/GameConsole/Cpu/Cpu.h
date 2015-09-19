/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>
#include "CpuOpCodes.h"
#include "CpuRegisters.h"

//#define CPU_CONTEXT_SIZE                  8

#define BEGIN_PC_HIGH_PART_VALUE_BUS_ADDR 0xFFFD
#define BEGIN_PC_LOW_PART_VALUE_BUS_ADDR  0xFFFC
#define BASE_STACK_ADDR                   0x0100


#define SET_BIT        1
#define CLR_BIT        0

typedef void (*CpuBusWriteCallBack_t)   ( void * context, uint16_t busAddr, uint8_t busData );
typedef uint8_t (*CpuBusReadCallBack_t) ( void * context, uint16_t busAddr );


class Cpu_t
{
public:
    // ------ Initialization ----------
    void SetBusWriteCallBack( CpuBusWriteCallBack_t busWriteCallBack, void* pContext );
    void SetBusReadCallBack( CpuBusReadCallBack_t busReadCallBack, void* pContext );

    //  ------ For Save Games ---------------
    uint32_t DumpRegisters( uint8_t* pOutRegistersData );
    uint32_t LoadRegisters( const uint8_t* pInRegistersData );
            
    uint32_t DumpMemory( uint8_t* pData, uint16_t startAddr, uint32_t bytesCnt );
    uint32_t LoadToMemory( const uint8_t* pData, uint16_t startAddr, uint32_t bytesCnt );
  
    // -------- Interrupts ------------
    void Reset();
    void NonMaskableInterrupt();
    void InterruptRequest();
    //---------------------------------
    
    uint32_t ExecuteOneCommand();

private:
    //inline void CalcOverflowFlag( uint16_t execRes, uint16_t  acc, uint16_t  mem );
    inline void StackPush( uint8_t pushValue );
    inline uint8_t StackPop();

    inline void BusWrite( uint16_t addr, uint8_t value );
    inline uint8_t BusRead( uint16_t addr );
    
private:
    CpuBusWriteCallBack_t fp_BusWriteCallBack;
    CpuBusReadCallBack_t  fp_BusReadCallBack;
    void*                 m_Context;  
    
private:
    bool m_IsNonMaskableInterrupt;
    bool m_IsInterruptRequest;
    bool m_IsReset;
      
private:
    CpuRegisters_t m_Registers;
};

#endif //_CPU_H_


