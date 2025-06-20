#include "hal/led.h"
#include "services/anim.h"
#ifndef PLATFORM_QT
#include "STC8H.h"

static u8 slice;
static u8 level[9];   /* brightness per LED 0-8 */

void hal_red_pwm_set(u8 idx, u8 duty)
{
    if(idx < 9)
    {
        level[idx] = duty;
    }
}

/* Timer-1 interrupt: 500 Hz soft PWM slices */
void timer1_isr(void) interrupt 3 using 1
{
    u8 i;
    slice++;

    /* 8 LEDs on P3, 1 LED on P4.0 */
    for(i = 0; i < 9; i++)
    {
        if(i < 8)
        {
            if(slice < level[i])  P3 |=  (1u << i);
            else                  P3 &= ~(1u << i);
        }
        else
        {
            if(slice < level[8])  P4 |= 0x01;
            else                  P4 &= ~0x01;
        }
    }

    /* slice rollover every 2 ms ? drive animation engine */
    if(slice == 0)
    {
        anim_tick_2ms();
    }
}
#endif   /* PLATFORM_QT */
