#include "hal/led.h"
#include "services/led_group.h"      /* led_io_map */
#include "common/platform.h"

#define LED_ACTIVE_LEVEL   1 /* 1 = 高电平点亮；0 = 低电平点亮 */

/* ==== ★  硬件IO ★ ==== */
data volatile const LedIo_t red_group[LED_RED_GROUP_CNT] = {
	{1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7},
};					
data volatile const LedIo_t blue_group[LED_BLUE_GROUP_CNT] = {
	{1, 0}
};							   
/* ===================================== */

void led_init()
{
    P1M0 = 0xff; 
    P1M1 = 0x00; 
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
        default:
            break;
        }
        break;
    
    default:
        break;
    }
}

void hal_led_set_group(u8 grp, u8 level)
{
    volatile u8 grp_bk = grp;
    volatile u8 level_bk = level;
	volatile u8 i;
    volatile LedIo_t *grpArr;
    volatile u8 grpSize;
    
	if (grp_bk >= LED_GROUP_CNT) return;

    switch (grp_bk)
    {
    case 0: 
        grpArr = red_group;
        grpSize = LED_RED_GROUP_CNT;
        break;
    case 1: 
        grpArr = blue_group;
        grpSize = LED_BLUE_GROUP_CNT;
        break;
    default:
        break;
    }

    for(i=0; i<grpSize; i++) {
        hal_led_set_io(&grpArr[i], level_bk);
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
