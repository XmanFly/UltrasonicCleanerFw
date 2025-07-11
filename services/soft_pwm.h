#ifndef SOFT_PWM_H
#define SOFT_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"
#include "led_group.h"        /* 为获取 LED_TOTAL 宏 */

#define LED_NUM             LED_TOTAL   /* 逻辑 LED 总数 */
#define LED_LEVEL_MAX       128
#define PWM_FRAME_TICKS     LED_LEVEL_MAX         

void soft_pwm_init(void);
void soft_pwm_set_level(u8 id, u8 level_0_127);   /* 任意占空 */
void soft_pwm_tick(void);                     /* 1 ms 调用 */

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* SOFT_PWM_H */
