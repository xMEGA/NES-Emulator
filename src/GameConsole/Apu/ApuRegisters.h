/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _ApuRegisters_h_
#define _ApuRegisters_h_

#include <stdint.h>

#define APU_SET_BIT 1
#define APU_CLR_BIT 0


// ----------------------------------------- Общие регистры для всего APU ------------------------------------------
#define APU_CHANNEL_ENABLE_REG_ADDR          0x4015    // Адрес регистра включения/выключения отдельных каналов ( При записи по адресу 0x4015 )
#define APU_STATUS_REG_ADDR                  0x4015    // Адрес регистра статуса (  При чтении по адресу 0x4015 )
#define APU_FRAME_SEQUENCER_CONTROL_REG_ADDR 0x4017  
// ----------------------------------------------------------------------------------------------------------------

#define APU_REG_0_SQUARE_CHANNEL_1_ADDR      0x4000    // Адрес регистра управления 1-го канала аудиопроцессора
#define APU_REG_1_SQUARE_CHANNEL_1_ADDR      0x4001    // Адрес регистра управления генератором 1-го канала аудиопроцессора
#define APU_REG_2_SQUARE_CHANNEL_1_ADDR      0x4002    // Адрес регистра управления периодом 1-го канала аудиопроцессора ( младший байт )
#define APU_REG_3_SQUARE_CHANNEL_1_ADDR      0x4003    // Адрес регистра управления периодом 1-го канала аудиопроцессора ( старший байт )

#define APU_REG_0_SQUARE_CHANNEL_2_ADDR      0x4004    // Адрес регистра управления 2-го канала аудиопроцессора
#define APU_REG_1_SQUARE_CHANNEL_2_ADDR      0x4005    // Адрес регистра управления генератором 2-го канала аудиопроцессора
#define APU_REG_2_SQUARE_CHANNEL_2_ADDR      0x4006    // Адрес регистра управления периодом 2-го канала аудиопроцессора ( младший байт )
#define APU_REG_3_SQUARE_CHANNEL_2_ADDR      0x4007    // Адрес регистра управления периодом 2-го канала аудиопроцессора ( старший байт )
 
#define APU_REG_0_TRIANGLE_CHANNEL_ADDR      0x4008    // Адрес регистра управления 3-го канала аудиопроцессора
#define APU_REG_1_TRIANGLE_CHANNEL_ADDR      0x400A    // Адрес регистра управления периодом 3-го канала аудиопроцессора ( младший байт )
#define APU_REG_2_TRIANGLE_CHANNEL_ADDR      0x400B    // Адрес регистра управления периодом 3-го канала аудиопроцессора ( старший байт )

#define APU_REG_0_NOISE_CHANNEL_ADDR         0x400C    // Адрес регистра 0 4-го канала аудиопроцессора
#define APU_REG_1_NOISE_CHANNEL_ADDR         0x400E    // Адрес регистра 1 4-го канала аудиопроцессора
#define APU_REG_2_NOISE_CHANNEL_ADDR         0x400F    // Адрес регистра 2 4-го канала аудиопроцессора


// ----------------------------------------- Общие регистры для всего APU ------------------------------------------

union ApuChannelEnableReg_t          // Регистр включения/выключения отдельных каналов ( При записи по адресу 0x4015 )
{
    struct
    {
        uint8_t EnableSquareCh1   : 1;
        uint8_t EnableSquareCh2   : 1;
        uint8_t EnableTriangleCh  : 1;
        uint8_t EnableNoiseCh     : 1;
        uint8_t EnableDeltaModCh  : 1;
        uint8_t                   : 3;
    };
    uint8_t Value;  
};

union ApuStatusReg_t                  // Регистр статуса APU ( При чтении по адресу 0x4015 )
{
    struct
    {
        uint8_t UpZeroLengthCtrSquareCh1  : 1; // Square Channel 1 length counter > 0
        uint8_t UpZeroLengthCtrSquareCh2  : 1; // Square Channel 2 length counter > 0
        uint8_t UpZeroLengthCtrTriangleCh : 1; // Triangle length counter > 0
        uint8_t UpZeroLengthCtrNoise      : 1; // Noise length counter > 0
        uint8_t UpZeroLengthCtrDeltaMod   : 1; // Delta modulation length counter > 0
        uint8_t                           : 1;
        uint8_t FrameIrqStatus            : 1; // Active when set
        uint8_t DeltaModIrqStatus         : 1; // Active when set
    };
    uint8_t Value;
};

union ApuFrameSequencerReg_t          // Регистр управления Frame Sequencer-ом   ( При записи по адресу 0x4017 )
{
    struct 
    {
        uint8_t                      : 6;
        uint8_t     IrqDisable       : 1;
        uint8_t     Mode             : 1; // Режим работы Frame Sequencer: 0 - 4-step mode, 1 - 5-step mode
    };
    uint8_t Value;
};

// ----------------------------------------------------------------------------------------------------------------


union ApuSquareControlReg_t        // ( При записи по адресам 0x4000/0x4004/0x4007 )
{
    struct
    {
    uint8_t Amplitude            : 4;
    uint8_t                      : 1;
    uint8_t LenCounterClkDisable : 1; 
    uint8_t DutyCycle            : 2; //Скважность сигнала
    };
    uint8_t Value;
};



union ApuEnvelopeReg_t                // ( При записи по адресам 0x4000/0x4004/0x4007 )
{
    struct
    {
        uint8_t Period               : 4;
        uint8_t Constant             : 1;
        uint8_t LoopEnable           : 1; 
        uint8_t                      : 2; 
    };
    uint8_t Value;
};


    
union ApuSquareSweepReg_t    // Регистр управления SweepUnit ( При записи по адресам 0x4001/0x4005 для каналов Square 1, Square 2 соответственно )
{
    struct
    {
        uint8_t SweepStep            : 3; // Шаг с которым изменяется частота с периодичностью SweepPeriod
        uint8_t SweepNegative        : 1; // 0 - увеличение на SweepStep, 1 - уменьшение на SweepStep
        uint8_t SweepPeriod          : 3; // Период ( в тактах SweepUnit ), с которым изменяется частота
        uint8_t SweepEnable          : 1; // 1 - Sweep разрешен, 0 - Sweep запрещен
    };
    uint8_t Value;
};

union ApuTriangleReg0_t                // ( При записи по адресу 0x4008 )
{
    struct
    {
        uint8_t LinearCounterLoad    : 7;
        uint8_t Control              : 1;
    };
    uint8_t Value;
};



union ApuNoiseControlReg_t        // ( При записи по адресу 0x400С )
{
    struct
    {
        uint8_t Amplitude            : 4;
        uint8_t                      : 1;
        uint8_t LenCounterClkDisable : 1; 
        uint8_t                      : 2; 
    };
    uint8_t Value;
};

union ApuNoiseReg1_t        // ( При записи по адресу 0x400B )
{
    struct
    {
        uint8_t    PeriodIdx         : 4;
        uint8_t                      : 3;
        uint8_t    Mode              : 1; 
    };
    uint8_t Value;
};


union ApuPeriodReg_t
{
    struct
    {
        uint8_t lowPart;
        uint8_t highPart;
    };
    uint16_t value;
};


#endif