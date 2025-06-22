#include "hal/led.h"
#include "services/anim.h"
#include <intrins.h>

/* === 用户可修改的硬件映射 ============================= */
#define FOSC_HZ        12000000UL   /* 12 MHz */
#define UNIT_TICKS     79           /* 79 µs per BCM unit (100 Hz frame) */
#define SEGMENTS       7            /* 7 bits → 128 级 */

/* 红灯 0‑7 : P1.0‑P1.7,   红灯 8 : P3.0,   蓝灯 : P3.1 */
#define P1_RED_MASK    0xFF
#define P3_RED_BIT     0
#define P3_BLUE_BIT    1
/* ======================================================= */

/* 计算 7 段重装载值  (12 T → 1 µs / tick) */
static const u16 reload_tbl[SEGMENTS] = {
    65536 - (UNIT_TICKS << 0),   /*  79 */
    65536 - (UNIT_TICKS << 1),   /* 158 */
    65536 - (UNIT_TICKS << 2),   /* 316 */
    65536 - (UNIT_TICKS << 3),   /* 632 */
    65536 - (UNIT_TICKS << 4),   /* 1 264 */
    65536 - (UNIT_TICKS << 5),   /* 2 528 */
    65536 - (UNIT_TICKS << 6)    /* 5 056 */
};

static u8 cur_bit;          /* 0‑6 */
static u8 red_lvl, blue_lvl;

/* 封装写 T3 重装 */
static void reload_t3(u16 val)
{
    T3H = val >> 8;
    T3L = val & 0xFF;
}

void led_hw_init(void)
{
    /* GPIO → 推挽 */
    P1M0 |= P1_RED_MASK;   
	P1M1 &= ~P1_RED_MASK;
    P3M0 |= (1<<P3_RED_BIT) | (1<<P3_BLUE_BIT);
    P3M1 &= ~((1<<P3_RED_BIT) | (1<<P3_BLUE_BIT));
	
	// P1 = ~P1_RED_MASK;   
    

    /* Timer‑3 置 12 T (默认)  */
    /* 某些头文件需设置 T34TCL 寄存器对应位, 示范如下: */
#ifdef T34TCL
    T34TCL &= ~(1<<3);          /* T3x12 = 0 → 12T */
#endif
    /* 装入首段 reload */
    reload_t3(reload_tbl[0]);
}

void led_hw_start(void)
{
    /* 清计数器 */
    T3H = T3L = 0;
    cur_bit = 0;

    IE2  |= 0x08;     /* ET3 = 1 (Timer‑3 interrupt enable) */
//    T3CON |= 0x04;    /* TR3 = 1  (start) */
}

/* Timer‑3 ISR (vector 0x00AB » Keil interrupt 19) */
void timer3_isr(void) interrupt 19
{
    /* 帧起点：更新本帧亮度 */
    if (cur_bit == 0) {
        red_lvl  = anim_get_level(LED_RED);
        blue_lvl = anim_get_level(LED_BLUE);
    }

    /* === 设 9 红灯 === */
    if (red_lvl & (1 << cur_bit)) {
        P1 |= P1_RED_MASK;
        P3 |= (1 << P3_RED_BIT);
    } else {
        P1 &= ~P1_RED_MASK;
        P3 &= ~(1 << P3_RED_BIT);
    }

    /* === 设 蓝灯 === */
    if (blue_lvl & (1 << cur_bit)) {
        P3 |= (1 << P3_BLUE_BIT);
    } else {
        P3 &= ~(1 << P3_BLUE_BIT);
    }

    /* 下一段 */
    reload_t3(reload_tbl[cur_bit]);

    if (++cur_bit >= SEGMENTS) cur_bit = 0;
}
