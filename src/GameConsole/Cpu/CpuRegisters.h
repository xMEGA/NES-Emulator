/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _CpuRegisters_h_
#define _CpuRegisters_h_

#include <stdint.h>

#define CPU_REG_PC_RESET_VALUE   0x0000
#define CPU_REG_AC_RESET_VALUE   0x00
#define CPU_REG_X_RESET_VALUE    0x00
#define CPU_REG_Y_RESET_VALUE    0x00
#define CPU_REG_SR_RESET_VALUE   0x00
#define CPU_REG_SP_RESET_VALUE   0xFF

#define SET_BIT      1
#define RESET_BIT    0

//#define    CPU_STACK_FENCE_ADDR    0x0100

struct CpuProgCounterReg_t
{
    union 
    {
        struct
        {
            uint8_t lowPart;
            uint8_t highPart;
        };
        uint16_t value;
    };
};

struct CpuStatusReg_t
{
    union 
    {
        struct
        {
            uint8_t C    :1;        //Carry
            uint8_t Z    :1;        //Zero
            uint8_t I    :1;        //Interrupt (IRQ disable)
            uint8_t D    :1;        //Decimal (use BCD for arithmetics)
            uint8_t B    :1;        //Break
            uint8_t Res  :1;        //Ignored
            uint8_t V    :1;        //Overflow
            uint8_t N    :1;        //Negative
        
        };
        uint8_t value;
    };
};

struct CpuRegisters_t
{
    uint8_t                AC;        
    uint8_t                X;
    uint8_t                Y;
    CpuProgCounterReg_t    PC;
    CpuStatusReg_t         SR;
    uint8_t                SP;
};

#endif