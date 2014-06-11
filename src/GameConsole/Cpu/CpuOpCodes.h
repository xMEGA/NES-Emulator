/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _CpuOpCodes_h_
#define _CpuOpCodes_h_

#include <stdint.h>


enum CpuAddressMode_t
{
    IMM_ADDR_MODE    = 0, //Immediate                 OPC #$BB         operand is byte (BB)

    REL_ADDR_MODE       , //Relative                     OPC $BB             branch target is PC + offset (BB), bit 7 signifies negative offset
    
    ACC_ADDR_MODE       , //Accumulator                 OPC A             operand is AC
    
    ABS_ADDR_MODE       , //Absolute                     OPC $HHLL         operand is address $HHLL
    ABS_X_ADDR_MODE     , //Absolute, X-indexed         OPC $HHLL,X         operand is address incremented by X with carry
    ABS_Y_ADDR_MODE     , //Absolute, Y-indexed         OPC $HHLL,Y         operand is address incremented by Y with carry
    
    ZP_ADDR_MODE        , //Zeropage                     OPC $LL             operand is of address; address hibyte = zero ($00xx)
    ZP_X_ADDR_MODE      , //Zeropage, X-indexed         OPC $LL,X         operand is address incremented by X; address hibyte = zero ($00xx); no page transition
    ZP_Y_ADDR_MODE      , //Zeropage, Y-indexed         OPC $LL,Y         operand is address incremented by Y; address hibyte = zero ($00xx); no page transition

    IND_ADDR_MODE       , //Indirect                     OPC ($HHLL)         operand is effective address; effective address is value of address
    IND_X_ADDR_MODE     , //X-indexed, indirect         OPC ($BB,X)         operand is effective zeropage address; effective address is byte (BB) incremented by X without carry
    IND_Y_ADDR_MODE     , //Indirect, Y-indexed         OPC ($LL),Y         operand is effective address incremented by Y with carry; effective address is word at zeropage address
        
    IMPL_ADDR_MODE      , //Implied                     OPC                 operand implied    
};

enum CpuMnemonic_t                    //addressing    assembler    opc  bytes  cyles
{
    ADC = 1,//....    add with carry
    AND,    //....    and (with accumulator)
    ASL,    //....    arithmetic shift left
    BCC,    //....    branch on carry clear
    BCS,    //....    branch on carry set
    BEQ,    //....    branch on equal (zero set)
    BIT,    //....    bit test
    BMI,    //....    branch on minus (negative set)
    BNE,    //....    branch on not equal (zero clear)
    BPL,    //....    branch on plus (negative clear)
    BRK,    //....    interrupt
    BVC,    //....    branch on overflow clear
    BVS,    //....    branch on overflow set
    CLC,    //....    clear carry
    CLD,    //....    clear decimal
    CLI,    //....    clear interrupt disable
    CLV,    //....    clear overflow
    CMP,    //....    compare (with accumulator)
    CPX,    //....    compare with X
    CPY,    //....    compare with Y
    DEC,    //....    decrement
    DEX,    //....    decrement X
    DEY,    //....    decrement Y
    EOR,    //....    exclusive or (with accumulator)
    INC,    //....    increment
    INX,    //....    increment X
    INY,    //....    increment Y
    JMP,    //....    jump
    JSR,    //....    jump subroutine
    LDA,    //....    load accumulator
    LDX,    //....    load X
    LDY,    //....    load Y
    LSR,    //....    logical shift right
    NOP,    //....    no operation
    ORA,    //....    or with accumulator
    PHA,    //....    push accumulator
    PHP,    //....    push processor status (SR)
    PLA,    //....    pull accumulator
    PLP,    //....    pull processor status (SR)
    ROL,    //....    rotate left
    ROR,    //....    rotate right
    RTI,    //....    return from interrupt
    RTS,    //....    return from subroutine
    SBC,    //....    subtract with carry
    SEC,    //....    set carry
    SED,    //....    set decimal
    SEI,    //....    set interrupt disable
    STA,    //....    store accumulator
    STX,    //....    store X
    STY,    //....    store Y
    TAX,    //....    transfer accumulator to X
    TAY,    //....    transfer accumulator to Y
    TSX,    //....    transfer stack pointer to X
    TXA,    //....    transfer X to accumulator
    TXS,    //....    transfer X to stack pointer
    TYA,    //....    transfer Y to accumulator
    // ---- Undocumented ----
    LAX,
    SAX,
    DCP,     
    ISB,
    SLO,
    RLA,
    SRE,
    RRA,
    // ---------------------
};

enum CpuOpCode_t                 /*addressing     assembler    opc  bytes  cyles*/
{
  ADC_IMM     = 0x69,          //immidiate     ADC #oper     69    2     2
  ADC_ZP      = 0x65,          //zeropage      ADC oper      65    2     3
  ADC_ZP_X    = 0x75,          //zeropage,X    ADC oper,X    75    2     4
  ADC_ABS     = 0x6D,          //absolute      ADC oper      6D    3     4
  ADC_ABS_X   = 0x7D,          //absolute,X    ADC oper,X    7D    3     4*
  ADC_ABS_Y   = 0x79,          //absolute,Y    ADC oper,Y    79    3     4*
  ADC_IND_X   = 0x61,          //(indirect,X)  ADC (oper,X)  61    2     6
  ADC_IND_Y   = 0x71,          //(indirect),Y  ADC (oper),Y  71    2     5*
  
  AND_IMM     = 0x29,         //immidiate     AND #oper     29    2     2
  AND_ZP      = 0x25,         //zeropage      AND oper      25    2     3
  AND_ZP_X    = 0x35,         //zeropage,X    AND oper,X    35    2     4
  AND_ABS     = 0x2D,         //absolute      AND oper      2D    3     4
  AND_ABS_X   = 0x3D,         //absolute,X    AND oper,X    3D    3     4*
  AND_ABS_Y   = 0x39,         //absolute,Y    AND oper,Y    39    3     4*
  AND_IND_X   = 0x21,         //(indirect,X)  AND (oper,X)  21    2     6
  AND_IND_Y   = 0x31,         //(indirect),Y  AND (oper),Y  31    2     5*
  
  ASL_ACC     = 0x0A,         //accumulator   ASL A         0A    1     2
  ASL_ZP      = 0x06,         //zeropage      ASL oper      06    2     5
  ASL_ZP_X    = 0x16,         //zeropage,X    ASL oper,X    16    2     6
  ASL_ABS     = 0x0E,         //absolute      ASL oper      0E    3     6
  ASL_ABS_X   = 0x1E,         //absolute,X    ASL oper,X    1E    3     7
  
  BCC_REL     = 0x90,         //relative      BCC oper      90    2     2**
  BCS_REL     = 0xB0,         //relative      BCS oper      B0    2     2**  
  BEQ_REL     = 0xF0,         //relative      BEQ oper      F0    2     2**
  BIT_ZP      = 0x24,         //zeropage      BIT oper      24    2     3
  BIT_ABS     = 0x2C,         //absolute      BIT oper      2C    3     4
  BMI_REL     = 0x30,         //relative      BMI oper      30    2     2**
  BNE_REL     = 0xD0,         //relative      BNE oper      D0    2     2**
  BPL_REL     = 0x10,         //relative      BPL oper      10    2     2**
  BRK_IMPL    = 0x00,         //implied       BRK           00    1     7
  BVC_REL     = 0x50,         //relative      BVC oper      50    2     2**
  BVS_REL     = 0x70,         //relative      BVC oper      70    2     2**
  CLC_IMPL    = 0x18,         //implied       CLC           18    1     2
  CLD_IMPL    = 0xD8,         //implied       CLD           D8    1     2
  CLI_IMPL    = 0x58,         //implied       CLI           58    1     2
  CLV_IMPL    = 0xB8,         //implied       CLV           B8    1     2
  
  CMP_IMM     = 0xC9,         //immidiate     CMP #oper     C9    2     2
  CMP_ZP      = 0xC5,         //zeropage      CMP oper      C5    2     3
  CMP_ZP_X    = 0xD5,         //zeropage,X    CMP oper,X    D5    2     4
  CMP_ABS     = 0xCD,         //absolute      CMP oper      CD    3     4
  CMP_ABS_X   = 0xDD,         //absolute,X    CMP oper,X    DD    3     4*
  CMP_ABS_Y   = 0xD9,         //absolute,Y    CMP oper,Y    D9    3     4*
  CMP_IND_X   = 0xC1,         //(indirect,X)  CMP (oper,X)  C1    2     6
  CMP_IND_Y   = 0xD1,         //(indirect),Y  CMP (oper),Y  D1    2     5*
  
  CPX_IMM     = 0xE0,         //immidiate     CPX #oper     E0    2     2
  CPX_ZP      = 0xE4,         //zeropage      CPX oper      E4    2     3
  CPX_ABS     = 0xEC,         //absolute      CPX oper      EC    3     4
  
  CPY_IMM     = 0xC0,         //immidiate     CPY #oper     C0    2     2
  CPY_ZP      = 0xC4,         //zeropage      CPY oper      C4    2     3
  CPY_ABS     = 0xCC,         //absolute      CPY oper      CC    3     4
  
  DEC_ZP      = 0xC6,         //zeropage      DEC oper      C6    2     5
  DEC_ZP_X    = 0xD6,         //zeropage,X    DEC oper,X    D6    2     6
  DEC_ABS     = 0xCE,         //absolute      DEC oper      CE    3     3
  DEC_ABS_X   = 0xDE,         //absolute,X    DEC oper,X    DE    3     7
  
  DEX_IMPL    = 0xCA,         //implied       DEC           CA    1     2
  DEY_IMPL    = 0x88,         //implied       DEC           88    1     2
  
  EOR_IMM     = 0x49,         //immidiate     EOR #oper     49    2     2
  EOR_ZP      = 0x45,         //zeropage      EOR oper      45    2     3
  EOR_ZP_X    = 0x55,         //zeropage,X    EOR oper,X    55    2     4
  EOR_ABS     = 0x4D,         //absolute      EOR oper      4D    3     4
  EOR_ABS_X   = 0x5D,         //absolute,X    EOR oper,X    5D    3     4*
  EOR_ABS_Y   = 0x59,         //absolute,Y    EOR oper,Y    59    3     4*    EOR_ IND, X 
  EOR_IND_X   = 0x41,         //(indirect,X)  EOR (oper,X)  41    2     6
  EOR_IND_Y   = 0x51,         //(indirect),Y  EOR (oper),Y  51    2     5*
  
  INC_ZP      = 0xE6,         //zeropage      INC oper      E6    2     5
  INC_ZP_X    = 0xF6,         //zeropage,X    INC oper,X    F6    2     6
  INC_ABS     = 0xEE,         //absolute      INC oper      EE    3     6
  INC_ABS_X   = 0xFE,         //absolute,X    INC oper,X    FE    3     7
  
  INX_IMPL    = 0xE8,         //implied       INX           E8    1     2
  INY_IMPL    = 0xC8,         //implied       INY           C8    1     2
  
  JMP_ABS     = 0x4C,         //absolute      JMP oper      4C    3     3
  JMP_IND     = 0x6C,         //indirect      JMP (oper)    6C    3     5
  
  JSR_ABS     = 0x20,         //absolute      JSR oper      20    3     6
  
  LDA_IMM     = 0xA9,         //immidiate     LDA #oper     A9    2     2
  LDA_ZP      = 0xA5,         //zeropage      LDA oper      A5    2     3
  LDA_ZP_X    = 0xB5,         //zeropage,X    LDA oper,X    B5    2     4
  LDA_ABS     = 0xAD,         //absolute      LDA oper      AD    3     4
  LDA_ABS_X   = 0xBD,         //absolute,X    LDA oper,X    BD    3     4*
  LDA_ABS_Y   = 0xB9,         //absolute,Y    LDA oper,Y    B9    3     4*
  LDA_IND_X   = 0xA1,         //(indirect,X)  LDA (oper,X)  A1    2     6
  LDA_IND_Y   = 0xB1,         //(indirect),Y  LDA (oper),Y  B1    2     5*
  
  LDX_IMM     = 0xA2,         //immidiate     LDX #oper     A2    2     2
  LDX_ZP      = 0xA6,         //zeropage      LDX oper      A6    2     3
  LDX_ZP_Y    = 0xB6,         //zeropage,Y    LDX oper,Y    B6    2     4
  LDX_ABS     = 0xAE,         //absolute      LDX oper      AE    3     4
  LDX_ABS_Y   = 0xBE,         //absolute,Y    LDX oper,Y    BE    3     4*
  
  LDY_IMM     = 0xA0,         //immidiate     LDY #oper     A0    2     2
  LDY_ZP      = 0xA4,         //zeropage      LDY oper      A4    2     3
  LDY_ZP_Y    = 0xB4,         //zeropage,X    LDY oper,X    B4    2     4
  LDY_ABS     = 0xAC,         //absolute      LDY oper      AC    3     4
  LDY_ABS_Y   = 0xBC,         //absolute,X    LDY oper,X    BC    3     4*
  
  LSR_ACC     = 0x4a,         //accumulator   LSR A         4A    1     2
  LSR_ZP      = 0x46,         //zeropage      LSR oper      46    2     5
  LSR_ZP_X    = 0x56,         //zeropage,X    LSR oper,X    56    2     6
  LSR_ABS     = 0x4E,         //absolute      LSR oper      4E    3     6
  LSR_ABS_X   = 0x5E,         //absolute,X    LSR oper,X    5E    3     7
  
  NOP_IMPL    = 0xEA,         //implied       NOP           EA    1     2
  
  ORA_IMM     = 0x09,         //immidiate     ORA #oper     09    2     2
  ORA_ZP      = 0x05,         //zeropage      ORA oper      05    2     3
  ORA_ZP_X    = 0x12,         //zeropage,X    ORA oper,X    15    2     4
  ORA_ABS     = 0x0D,         //absolute      ORA oper      0D    3     4
  ORA_ABS_X   = 0x1D,         //absolute,X    ORA oper,X    1D    3     4*
  ORA_ABS_Y   = 0x19,         //absolute,Y    ORA oper,Y    19    3     4*
  ORA_IND_X   = 0x01,         //(indirect,X)  ORA (oper,X)  01    2     6
  ORA_IND_Y   = 0x11,         //(indirect),Y  ORA (oper),Y  11    2     5*
  
  PHA_IMPL    = 0x48,         //implied       PHA           48    1     3
  PHP_IMPL    = 0x08,         //implied       PHP           08    1     3
  PLA_IMPL    = 0x68,         //implied       PLA           68    1     4
  PLP_IMPL    = 0x28,         //implied       PHP           28    1     4
  
  ROL_ACC     = 0x2A,         //accumulator   ROL A         2A    1     2
  ROL_ZP      = 0x26,         //zeropage      ROL oper      26    2     5
  ROL_ZP_X    = 0x36,         //zeropage,X    ROL oper,X    36    2     6
  ROL_ABS     = 0x2E,         //absolute      ROL oper      2E    3     6
  ROL_ABS_X   = 0x3E,         //absolute,X    ROL oper,X    3E    3     7
  
  ROR_ACC     = 0x6A,         //accumulator   ROR A         6A    1     2
  ROR_ZP      = 0x66,         //zeropage      ROR oper      66    2     5
  ROR_ZP_X    = 0x76,         //zeropage,X    ROR oper,X    76    2     6
  ROR_ABS     = 0x6E,         //absolute      ROR oper      6E    3     6
  ROR_ABS_X   = 0x7E,         //absolute,X    ROR oper,X    7E    3     7
  
  RTI_IMPL    = 0x40,         //implied       RTI           40    1     6
  RTS_IMPL    = 0x60,         //implied       RTS           60    1     6
  
  SBC_IMM     = 0xE9,         //immidiate     SBC #oper     E9    2     2
  SBC_ZP      = 0xE5,         //zeropage      SBC oper      E5    2     3
  SBC_ZP_X    = 0xF5,         //zeropage,X    SBC oper,X    F5    2     4
  SBC_ABS     = 0xED,         //absolute      SBC oper      ED    3     4
  SBC_ABS_X   = 0xFD,         //absolute,X    SBC oper,X    FD    3     4*
  SBC_ABS_Y   = 0xF9,         //absolute,Y    SBC oper,Y    F9    3     4*
  SBC_IND_X   = 0xE1,         //(indirect,X)  SBC (oper,X)  E1    2     6
  SBC_IND_Y   = 0xF1,         //(indirect),Y  SBC (oper),Y  F1    2     5*
  
  SEC_IMPL    = 0x38,         //implied       SEC           38    1     2
  SED_IMPL    = 0xF8,         //implied       SED           F8    1     2
  SEI_IMPL    = 0x78,         //implied       SEI           78    1     2
  
  STA_ZP      = 0x85,         //zeropage      STA oper      85    2     3
  STA_ZP_X    = 0x95,         //zeropage,X    STA oper,X    95    2     4
  STA_ABS     = 0x8D,         //absolute      STA oper      8D    3     4
  STA_ABS_X   = 0x9D,         //absolute,X    STA oper,X    9D    3     5
  STA_ABS_Y   = 0x99,         //absolute,Y    STA oper,Y    99    3     5
  STA_IND_X   = 0x81,         //(indirect,X)  STA (oper,X)  81    2     6
  STA_IND_Y   = 0x91,         //(indirect),Y  STA (oper),Y  91    2     6
  
  STX_ZP      = 0x86,         //zeropage      STX oper      86    2     3
  STX_ZP_Y    = 0x96,         //zeropage,Y    STX oper,Y    96    2     4
  STX_ABS     = 0x8E,         //absolute      STX oper      8E    3     4
  
  STY_ZP      = 0x84,         //zeropage      STY oper      84    2     3
  STY_ZP_X    = 0x94,         //zeropage,X    STY oper,X    94    2     4
  STY_ABS     = 0x8C,         //absolute      STY oper      8C    3     4
  
  TAX_IMPL    = 0xAA,         //implied       TAX           AA    1     2
  TAY_IMPL    = 0xA8,         //implied       TAY           A8    1     2
  TSX_IMPL    = 0xBA,         //implied       TSX           BA    1     2
  TXA_IMPL    = 0x8A,         //implied       TXA           8A    1     2
  TXS_IMPL    = 0x9A,         //implied       TXS           9A    1     2
  TYA_IMPL    = 0x98          //implied       TYA           98    1     2
};

#define LSB

#ifdef LSB

struct CpuDecodeTable_t
{
    uint32_t      mnemonic : 6;
    uint32_t      resWrite : 1;
    uint32_t      opRead   : 1;
    uint32_t      opBytes  : 2;
    uint32_t      opCycles : 3;
    uint32_t      addrMode : 4;
    //uint32_t              :15;
};

#else

struct CpuDecodeTable_t
{
    //uint32_t              :15;

    uint32_t      addrMode : 4;

    uint32_t      opCycles : 3;

    uint32_t      opBytes  : 2;

    uint32_t      opRead   : 1;

    uint32_t      resWrite : 1;

    uint32_t      mnemonic : 6;
  
};

#endif

          
extern const CpuDecodeTable_t cpuDecodeTable[256];

#endif