#include "hal/touch.h"
#ifndef PLATFORM_QT
#include "STC8H.h"

static volatile u8 ev;

void hal_touch_init(void)
{ 
    IT0=1;
    IE0=0;
    EX0=1;
}

void int0_isr(void) interrupt 0 using 2 
{ 
	EX0=0;
	ev=1;
	EX0=1;
}

u8 hal_touch_event(void)
{ 
	u8 r;
	EA=0;
	r=ev;
	ev=0;
	EA=1;
	return r;
}

#endif
