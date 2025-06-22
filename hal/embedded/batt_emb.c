#include "hal/battery.h"

#ifndef PLATFORM_QT
#include "STC8H.h"

void hal_batt_init(void)
{   
    P3M0 &= ~0x10; P3M1 &= ~0x10;  // P3.4输入 双向端口

    ADC_CONTR=0x80;
}

u16 hal_batt_get_mv(void)
{
    // u16 raw;

    // ADC_CONTR = 0xC8;
    // while(!(ADC_CONTR & 0x10)) {        
    // }
    // ADC_CONTR &= ~0x10;

    // raw  = ((u16)ADC_RES  << 2) | (ADC_RESL & 0x03);

    // return (u32)raw * 3300u / 1023u;
    
    return 5000;
}

u8 hal_batt_is_chg(void)
{
	return P34;
}

#endif
