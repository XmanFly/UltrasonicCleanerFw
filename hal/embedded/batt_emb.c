#include "hal/battery.h"

#ifndef PLATFORM_QT
#include "STC8H.h"

void hal_batt_init(void)
{    
    ADC_CONTR=0x80;
}

u16 hal_batt_get_mv(void)
{
    u16 raw;

    ADC_CONTR = 0xC8;
    while(!(ADC_CONTR & 0x10)) {        
    }
    ADC_CONTR &= ~0x10;

    raw  = ((u16)ADC_RES  << 2) | (ADC_RESL & 0x03);

    return (u32)raw * 3300u / 1023u;
}

u8 hal_batt_is_chg(void)
{
	return (P1&0x01)?1:0;
}

#endif
