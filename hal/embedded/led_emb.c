#include "hal/led.h"

#ifndef PLATFORM_QT
#include "STC8H.h"

void hal_blue_pwm_set(u8 duty)
{
	u16 cmp = ((u16)duty * 125) / 255;
//	PWMAH   = (cmp >> 8) & 0x03;
//	PWMAL   = cmp & 0xFF;
}


#endif
