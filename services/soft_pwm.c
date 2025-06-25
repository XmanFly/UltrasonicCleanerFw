#include "soft_pwm.h"
#include "hal/led.h"
#include "common/platform.h"

typedef struct {
    u8 target;      /* 0-127 */
    u8 err;         /* 3-bit 余数 */
    u8 duty;        /* 本帧占空 0-15 */
} pwm_t;

static volatile pwm_t led[LED_NUM];
static volatile u8     tick = 0;                /* 0-15 */

void soft_pwm_init(void)
{
    u8 i;
    for (i = 0; i < LED_NUM; ++i) {
        led[i].target = 0;
        led[i].err    = 0;
        led[i].duty   = 0;
        led_off(i);
    }
}

void soft_pwm_set_level(u8 id, u8 lv)
{
    if (id < LED_NUM) {
        if (lv > LED_LEVEL_MAX) lv = LED_LEVEL_MAX;
        led[id].target = lv;
    }
    // print("soft_pwm_set_level id %bu lv %bu\n", id, lv);
}

void soft_pwm_tick_1ms(void)
{
    volatile u8 i;
    if (tick == 0) {                    /* 帧起点：Bresenham, 此处保证了16帧里面数值不变 */
        for (i = 0; i < LED_NUM; ++i) {
            u16 acc = (u16)led[i].err + led[i].target;   /* 0-134 */
            led[i].duty = (u8)(acc >> 3);                /* ÷8→0-15 */
            led[i].err  = (u8)(acc & 0x07);              /* 低 3 位 */
        }
    }

    for (i = 0; i < LED_NUM; ++i) {
        // 亮度为0 特殊处理
        if(led[i].duty == 0) {
            led_off(i);
            continue;
        }
        if (tick <= led[i].duty)  {            
            led_on(i);
        } else {
            led_off(i);
        }                              
    }

    if (++tick >= PWM_FRAME_TICKS) tick = 0;
}

void led_set_level(u8 id, u8 lv_0_15)
{
    soft_pwm_set_level(id, (u8)(lv_0_15 << 3));   /* ×8 */
}
