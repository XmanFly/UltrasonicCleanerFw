#include "hal/led.h"
#include "services/led_group.h"      /* led_io_map */
#include "common/platform.h"

#define LED_ACTIVE_LEVEL   1 /* 1 = 高电平点亮；0 = 低电平点亮 */

/* ==== ★  硬件IO ★ ==== */
const LedIo_t led_io_map[LED_TOTAL] = {
	{1, 2}, {1, 3}, {1, 4},{1, 5},{1, 6}, {1, 7},
    {1, 0}
};											   
/* ===================================== */

void led_init()
{
    P1M0 = 0xff; 
    P1M1 = 0x00; 
    P13 = 0;
}

void hal_led_set(u8 id, u8 level)
{
	u8 p, m;
	if (id >= LED_TOTAL) return;
	p = led_io_map[id].port_no;
	m = led_io_map[id].mask;	

    print("hal_led_set id %bu level %bu\n", id, level);

    switch (p)
    {
    case 1:
        switch (m)
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

void led_on(u8 id)
{
    if (id >= LED_TOTAL) return;
    hal_led_set(id, LED_ACTIVE_LEVEL);
}

void led_off(u8 id)
{
    if (id >= LED_TOTAL) return;
    hal_led_set(id, !LED_ACTIVE_LEVEL);
}
