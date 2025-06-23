#include "hal/led.h"
#include "services/led_group.h"      /* led_io_map */

#define LED_ACTIVE_LEVEL   1 /* 1 = 高电平点亮；0 = 低电平点亮 */

/* ==== ★  硬件IO ★ ==== */
const LedIo_t led_io_map[LED_TOTAL] = {
	{1, 1}, {1, 2}
};											   
/* ===================================== */

void hal_led_set(u8 id, u8 level)
{
	u8 p, m;
	if (id >= LED_TOTAL) return;
	p = led_io_map[id].port_no;
	m = led_io_map[id].mask;	

    switch (p) {
    case 1: level ? (P1 |=  (1 << p)) : (P1 &= ~(1 << p)); break;
    case 2: level ? (P2 |=  (1 << p)) : (P2 &= ~(1 << p)); break;
    case 3: level ? (P3 |=  (1 << p)) : (P3 &= ~(1 << p)); break;
    case 4: level ? (P4 |=  (1 << p)) : (P4 &= ~(1 << p)); break;
    default: break;
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
