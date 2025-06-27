#include "common/platform.h"
#include "hal/power.h"

void power_init()
{
    P3M0 |= 0x80; 
	P3M1 &= ~0x80; 
}
	
void power_on ()
{
	P37 = 1;
	print("power_on\n");
}

void power_off()
{
	P37 = 0;
	print("power_off\n");
}
