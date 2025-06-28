#include "hal/led.h"
#include "services/led_group.h"      /* led_io_map */
#include "common/platform.h"

#define LED_ACTIVE_LEVEL   1 /* 1 = 高电平点亮；0 = 低电平点亮 */


/* ==== ★  硬件IO ★ ==== */
data volatile const LedIo_t red_group[LED_RED_GROUP_CNT] = {
#ifdef RELEASE    
	{1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {3, 1}, {5, 4}
#else
    {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {5, 4}
#endif
};					
data volatile const LedIo_t blue_group[LED_BLUE_GROUP_CNT] = {
	{1, 0}
};							   
/* ===================================== */

void led_init()
{
    P1M0 = 0xff; 
    P1M1 = 0x00; 

#ifndef UART_ENABLE
    P3M0 |= 0x02; 
    P3M1 &= ~0x02; 
#endif

    P5M0 |= 0x10; 
    P5M1 &= ~0x10; 
}

void hal_led_set_io(LedIo_t *io, u8 level)
{
    switch (io->port_no)
    {
    case 1:
        switch (io->mask)
        {
        case 0: P10 = level; break;
        case 1: P11 = level; break;
        case 2: P12 = level; break;
        case 3: P13 = level; break;
        case 4: P14 = level; break;
        case 5: P15 = level; break;
        case 6: P16 = level; break;
        case 7: P17 = level; break;
        default: break;
        }
        break;
    case 3:
        switch (io->mask)
        {
        case 0: P30 = level; break;
        case 1: P31 = level; break;
        case 2: P32 = level; break;
        case 3: P33 = level; break;
        case 4: P34 = level; break;
        case 5: P35 = level; break;
        case 6: P36 = level; break;
        case 7: P37 = level; break;
        default: break;
        }
        break;        
    case 5:
        switch (io->mask)
        {
        case 0: P50 = level; break;
        case 1: P51 = level; break;
        case 2: P52 = level; break;
        case 3: P53 = level; break;
        case 4: P54 = level; break;
        case 5: P55 = level; break;
        case 6: P56 = level; break;
        case 7: P57 = level; break;                
        default: break;
        }
        break;    
    default:
        break;
    }
}

void hal_led_set_group(u8 grp, u8 level)
{
	if (grp >= LED_GROUP_CNT) return;

    switch (grp)
    {
    case 0: 
        if(level) {
            P12 = 1;
            P13 = 1;
            P14 = 1;
            P15 = 1;
            P16 = 1;
            P17 = 1;
            P54 = 1;
#ifdef RELEASE  
            P11 = 1;
            P31 = 1;
#endif
        } else {
            P12 = 0;
            P13 = 0;
            P14 = 0;
            P15 = 0;
            P16 = 0;
            P17 = 0;
            P54 = 0;
#ifdef RELEASE  
            P11 = 0;
            P31 = 0;
#endif            
        }
        break;
    case 1: 
        if(level) {
            P10 = 1;
        } else {
            P10 = 0;
        }
        break;
    default:
        break;
    }

    // print("hal_led_set id %bu level %bu\n", grp, level);    
}

void led_on(u8 grp)
{
    if (grp >= LED_GROUP_CNT) return;
    hal_led_set_group(grp, LED_ACTIVE_LEVEL);
}

void led_off(u8 grp)
{
    if (grp >= LED_GROUP_CNT) return;
    hal_led_set_group(grp, !LED_ACTIVE_LEVEL);
}
