#include "hal/led.h"

#ifndef PLATFORM_QT
#include "STC8H.h"

static u8 slice;
static u8 level[9];

void hal_red_pwm_set(u8 idx,u8 duty)
{  
    if(idx<9) level[idx]=duty;
}

void timer1_isr(void) interrupt 3 using 1
{
	u8 i;
	slice++;
	for(i=0;i<9;i++){
		if(i<8){
			if(slice<level[i]) P3|=(1u<<i);
			else P3&=~(1u<<i);
		} else {
			if(slice<level[8]) P4|=0x01;
			else P4&=~0x01;
		}
	}
}
#endif
