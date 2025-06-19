
#include "hal/ultrasonic.h"
#ifndef PLATFORM_QT
#include "STC8H.h"
static const u16 tab[21]={58290,58342,58393,58444,58494,58544,58594,58644,58694,
58744,58793,58842,58891,58939,58988,59036,59083,59131,
59178,59225,59271};
static u8 idx;

void hal_us_start(void)
{
//	u16 u16;
//	idx=0;
//	T2L=rc&0xFF;
//	T2H=rc>>8;
//	CCAPL=(rc>>1)&0xFF;
//	CCAPH=(rc>>1)>>8;
//	CCON|=0x40;
//	T2CON|=0x04;
}

void hal_us_stop(void)
{	
//	T2CON&=~0x04;
//	CCON&=~0x40;
}
#endif
