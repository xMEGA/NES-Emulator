/*
    NES Emulator Project
    Created by Igor Belorus
    06/11/2012
*/

#include "PpuRegisters.h"

//void PpuRegisters_t::Reset(void)
//{
//    //C1=0;
////    C2=0;        
////    SR=0;
//
//
//    AS=0;
//    DS=0;
//    HS.HorizontalValue = 0;
//    HS.VerticalValue   = 0;
//    AV.value=0;
//    DV=0;
//    m_VideoRamAddrRegHighPart = true;
//    m_VerticalScrollingValue  = false;
//
//    
//}


//void PpuRegisters_t::WriteVideoRamAddrReg(uint8_t halfAddr)
//{
//    if( m_VideoRamAddrRegHighPart )
//    {
//        AV.value = halfAddr << 8;
//        m_VideoRamAddrRegHighPart = false;
//    }
//    else
//    {
//        AV.value |= halfAddr;
//        m_VideoRamAddrRegHighPart = true;
//    }
//
//}

//void PpuRegisters_t::WriteScrollingReg(uint8_t value)
//{
//    if( m_VerticalScrollingValue )
//    {
//        HS.VerticalValue = value;
//        m_VerticalScrollingValue = false;
//    }
//    else
//    {
//        HS.HorizontalValue = value;
//        m_VerticalScrollingValue = true;
//    }
//}
