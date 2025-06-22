#include "hal/touch.h"
#ifndef PLATFORM_QT
#include "STC8H.h"

/* edge event flag */
static volatile u8 event_flag = 0;
/* realtime level: 1 = pressed */
static volatile u8 key_level  = 0;

void hal_touch_init(void)
{
    // P3.2配置成输入口
    P3M0 &= ~0x04; P3M1 |= 0x04; 

    // 中断
    IT0 = 1;     /* interrupt on falling edge */
    IE0 = 0;
    EX0 = 1;
}

void int0_isr(void) interrupt 0 using 2
{
    EX0 = 0;
    event_flag = 1;   /* edge detected */
    key_level  = 1;   /* still pressed */
    IE0 = 0;          /* clear INT flag */
    EX0 = 1;
}

u8 hal_touch_is_pressed(void)
{
    /* P3.2 active-low key (modify if your pin differs) */
    return (P3 & 0x04) ? 0 : 1;
}
#endif   /* PLATFORM_QT */
