#include "hal/led.h"

#ifdef PLATFORM_QT
static u8 blue;
static u8 red[9];

void hal_blue_pwm_set(u8 duty)
{
    blue = duty;
}

void hal_red_pwm_set(u8 idx, u8 duty)
{
    if(idx < 9) {
        red[idx] = duty;
    }
}

u8 sim_get_blue(void)
{
    return blue;
}

u8 sim_get_red(u8 idx)
{
    return (idx < 9) ? red[idx] : 0;
}
#endif
