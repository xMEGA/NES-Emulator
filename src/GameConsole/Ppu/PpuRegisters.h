/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#ifndef _PpuRegisters_h_
#define _PpuRegisters_h_

#include <stdint.h>

#define PPU_SET_BIT 1
#define PPU_CLR_BIT 0


#define PPU_REG_CONTROL_1_ADDR          0x2000    // Адрес Регистра 1 управления видеопроцессором
#define PPU_REG_CONTROL_2_ADDR          0x2001    // Адрес Регистра 2 управления видеопроцессором
#define PPU_REG_STATUS_PPU_ADDR         0x2002    // Адрес Регистра состояния видеопроцессора
#define PPU_REG_ADDR_SPRITES_ADDR       0x2003    // Адрес Регистра адреса памяти спрайтов
#define PPU_REG_DATA_SPRITES_ADDR       0x2004    // Адрес Регистра данных памяти спрайтов
#define PPU_REG_HARDWARE_SCROLL_ADDR    0x2005    // Адрес Регистра аппаратной прокрутки
#define PPU_REG_ADDR_VRAM_ADDR          0x2006    // Адрес Регистра адреса видеопамяти
#define PPU_REG_DATA_VRAM_ADDR          0x2007    // Адрес Регистра данных видеопамяти


enum ActiveScreenPage_t
{
    PPU_SCREEN_PAGE1_ACTIVE = (uint8_t) 0,
    PPU_SCREEN_PAGE2_ACTIVE = (uint8_t) 1,
    PPU_SCREEN_PAGE3_ACTIVE = (uint8_t) 2,
    PPU_SCREEN_PAGE4_ACTIVE = (uint8_t) 3
};

union PpuCtrlReg1_t
{    
    struct
    {
        uint8_t ActiveScreenPage     : 2; // Адрес активной экранной страницы ( 00 - 0x2000, 01 - 0x2400, 10 - 0x2800 , 11 - 0x2C00 )
        uint8_t IncVideoRamAddr      : 1; // Приращение адреса при обращении к видеопамяти ( 0 - увеличениие адреса на 1, 1 - увеличение адреса на 32 )
        uint8_t SpriteChrBaseAddr    : 1; // Базовый адрес знакогенератора для формирования спрайтов ( 0 - 0x0000, 1 - 0x1000 )
        uint8_t FoneChrBaseAddr      : 1; // Базовый адрес знакогенератора для формирования фона ( 0 - 0x0000, 1 - 0x1000 )
        uint8_t SpriteSize           : 1; // Размер спрайта ( 0 - 8x8 точек, 1 - 8x16 точек )
        uint8_t NotUsedBit           : 1; // Не используется
        uint8_t NmiRequestEnable     : 1; // Разрешение формирования запроса NMI при кадровом синхоимпульсе
    };
    uint8_t Value;
};
    
union PpuCtrlReg2_t
{
    struct
    {
        uint8_t DisplayType          : 1; // Тип дисплея (не используется)
        uint8_t FoneVisibleRegion    : 1; // Область видимости фона ( 0 - фон невидим в крайнем левом столбце из 8 точек, 1 - фон присутствует на всем экране )
        uint8_t SpritesVisibleRegion : 1; // Область видимости спрайтов ( 0 - спрайты невидимы в крайнем левом столбце из 8 точек, 1 - спрайты присутствуют на всем экране )
        uint8_t FoneVisibleEnable    : 1; // Управление отображением фона ( 0 - фон не отображается, 1 - фон отображается )
        uint8_t SpritesVisibleEnable : 1; // Управление отображением спрайтов ( 0 - спрайты не отображаются, 1 - спрайты отображаются )
        uint8_t Brightness           : 3; // Яркость экрана (не используется)
    };
    uint8_t Value;
};

union PpuStatusReg_t
{

    struct
    {
        uint8_t NotUsedBits          : 4; // Не используются
        uint8_t VideoRamWriteDisable : 1; // Разрешение записи в видеопамять ( 0 - запись разрешена, 1 - запись запрещена )
        uint8_t SpriteCountPerLine   : 1; // Счетчик количества спрайтов на текущей линии ( 1 - больше 8 стпрайтов на линии, 0 - меньше либо равно 8 спрайтов на линии )
        uint8_t ZeroSpriteDetected   : 1; // Флаг обнаружения спрайта 0 ( 0 - спрайт 0 не обнаружен, 1- спрайт 0 выведен на экран )
        uint8_t VsyncFlag            : 1; // Флаг кадрового синхроимпульса ( 0 - видеопроцессор формирует изображение, 1 - видеопроцессор генерирует кадровый синхроимпульс )
    };
    uint8_t Value;

};

struct PpuScrollingReg_t
{
    uint8_t VerticalValue;
    uint8_t HorizontalValue;
};

union PpuAddrVideoReg_t
{
    struct
    {
        uint8_t lowPart;
        uint8_t highPart;
    };
    uint16_t value;
};


struct PpuRegisters_t
{
  //  public:
        //void Reset(void);
        //void WriteVideoRamAddrReg( uint8_t halfAddr ); // Записывает адрес в регистр AV за два захода
        //void WriteScrollingReg( uint8_t value ); // Записывает значение прокрутки изображения за два захода ( сначала вертикальная прокрутка, затем - горизонтальная )
 //   public:
        PpuCtrlReg1_t       C1;
        PpuCtrlReg2_t       C2;        
        PpuStatusReg_t      SR;
        uint8_t             AS;
        //uint8_t             DS;
        //PpuScrollingReg_t   HS;
        uint16_t            AV;
       // uint8_t             DV;
    
   /* private:
        bool m_VideoRamAddrRegHighPart;
        bool m_VerticalScrollingValue;*/
};


#endif