#include "hal/time.h"

#ifndef PLATFORM_QT
#include "STC8H.h"

extern void fsm_tick_1ms(void);
extern void soft_timer_tick_1ms(void);

void hal_time_init(void)
{
    TMOD&=~0x03;
    TMOD|=0x01;
    TH0=0xE1;
    TL0=0x00;
    /* 1 ms @12 MHz */;
    ET0=1;
    TR0=1;
}

void timer0_isr(void) interrupt 1 using 1
{
	TH0=0xE1;
	TL0=0x00;
	soft_timer_tick_1ms();
}

void hal_time_tick_1ms(void)
{    
	fsm_tick_1ms();
    soft_timer_tick_1ms();
}
#endif
