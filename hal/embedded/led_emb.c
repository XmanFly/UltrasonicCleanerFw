#include "hal/led.h"
#include "services/led_group.h"      /* led_io_map */

#define LED_ACTIVE_LEVEL   1 /* 1 = 高电平点亮；0 = 低电平点亮 */

/* ==== ★  根据实际硬件改以下两行 ★ ==== */
/* 假设逻辑 IO 编号 0..31 对应 P1.0~P1.7、P2.0~P2.7、P3.0~P3.7、P4.0~P4.7 */
static code unsigned char port_tab[] = {1,1,1,1,1,1,1,1,  /* P1 */
                                               2,2,2,2,2,2,2,2,  /* P2 */
                                               3,3,3,3,3,3,3,3,  /* P3 */
                                               4,4,4,4,4,4,4,4}; /* P4 */
static code unsigned char pin_tab [] = {0,1,2,3,4,5,6,7,
                                               0,1,2,3,4,5,6,7,
                                               0,1,2,3,4,5,6,7,
                                               0,1,2,3,4,5,6,7};
/* ===================================== */

void hal_led_set(u8 io, u8 level)
{
    if (io >= sizeof(port_tab)) return;  /* 越界保护 */

    switch (port_tab[io]) {
    case 1: level ? (P1 |=  (1 << pin_tab[io])) : (P1 &= ~(1 << pin_tab[io])); break;
    case 2: level ? (P2 |=  (1 << pin_tab[io])) : (P2 &= ~(1 << pin_tab[io])); break;
    case 3: level ? (P3 |=  (1 << pin_tab[io])) : (P3 &= ~(1 << pin_tab[io])); break;
    case 4: level ? (P4 |=  (1 << pin_tab[io])) : (P4 &= ~(1 << pin_tab[io])); break;
    default: break;
    }
}

static void gpio_write(u8 io, u8 lv)
{
    if (io == 0xFF) return;          /* 占位无效 */
    hal_led_set(io, lv);
}

void led_on(u8 id)
{
    if (id >= LED_TOTAL) return;
    gpio_write(led_io_map[id], LED_ACTIVE_LEVEL);
}

void led_off(u8 id)
{
    if (id >= LED_TOTAL) return;
    gpio_write(led_io_map[id], !LED_ACTIVE_LEVEL);
}
