#include "soft_pwm.h"
#include "hal/led.h"
#include "common/platform.h"

typedef struct {
    u8 target;      /* 0-15 */
    u8 duty;        /* 本帧占空 0-15 */
} pwm_t;

static pwm_t ledGroup[LED_GROUP_CNT];
static u8     tick = 0;                /* 0-15 */

void soft_pwm_init(void)
{
    u8 i;
    for (i = 0; i < LED_GROUP_CNT; ++i) {
        ledGroup[i].target = 0;
        ledGroup[i].duty   = 0;
        led_off(i);
    }
}

void soft_pwm_set_level(u8 id, u8 lv)
{
    if (id < LED_GROUP_CNT) {
        if (lv > LED_LEVEL_MAX) lv = LED_LEVEL_MAX;
        ledGroup[id].target = lv;
    }
    // print("soft_pwm_set_level id %bu lv %bu\n", id, lv);
}

void soft_pwm_tick_1ms(void)
{
    volatile u8 i;
    if (tick == 0) {                    /* 帧起点, 此处保证了16帧里面数值不变 */
        for (i = 0; i < LED_GROUP_CNT; ++i) {
            ledGroup[i].duty = ledGroup[i].target; /* ÷8→0-15 */
        }
    }

    for (i = 0; i < LED_GROUP_CNT; ++i) {
        // 亮度为0 特殊处理
        if(ledGroup[i].duty == 0) {
            led_off(i);
            continue;
        }
        if (tick <= ledGroup[i].duty)  {            
            led_on(i);
        } else {
            led_off(i);
        }                              
    }

    if (++tick >= PWM_FRAME_TICKS) tick = 0;
}
