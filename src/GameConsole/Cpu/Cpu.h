/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _Cpu_h_
#define _Cpu_h_

#include <stdint.h>
#include "CpuOpCodes.h"
#include "CpuRegisters.h"

#define _in_
#define _out_
#define _in_out_

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
    void SetBusWriteCallBack( CpuBusWriteCallBack_t busWriteCallBack, void * pContext );
    void SetBusReadCallBack( CpuBusReadCallBack_t busReadCallBack, void * pContext );

    //  ------ For Save Games ---------------
    void SaveCpuContext( uint8_t* pOutData );
    void LoadCpuContext( uint8_t* pInData );
    // --------------------------------------

    // -------- Interrupts ------------
    void Reset(void);
    void NonMaskableInterrupt(void);
    void InterruptRequest(void);
    //---------------------------------
    
    uint8_t Run();

private:
    inline void CalcZeroFlag( uint16_t n );
    inline void CalcCarryFlag( uint16_t n );
    inline void CalcSignFlag( uint16_t n);
    inline void CalcOverflowFlag( uint16_t execRes, uint16_t  acc, uint16_t  mem );
    inline void StackPush(uint8_t pushval);
    inline uint8_t StackPop( void );
    inline void AddressCalc( void );
    inline void CommandExecute(void);

private:
    CpuBusWriteCallBack_t fp_BusWriteCallBack;
    CpuBusReadCallBack_t  fp_BusReadCallBack;

private:
    CpuRegisters_t              m_Registers;
    uint8_t                     m_Cycles;
    uint8_t                     m_SaveValue;
    uint16_t                    m_ExecuteResult;
    void*                       m_Context;
    uint16_t                    m_FetchWord;
    CpuDecodeTable_t            m_CpuDecodeInfo;
    CpuProgCounterReg_t         m_OperandAddr;
    uint16_t                    m_OperandValue;
    bool                        m_OpValReadNeed;
    bool                        m_AdditionCycleEnable;
    bool                        m_IsNonMaskableInterrupt;
    bool                        m_IsInterruptRequest;
    bool                        m_IsReset;
};

#endif


