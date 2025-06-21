#ifndef SERVICES_BLUE_LED_SM_H
#define SERVICES_BLUE_LED_SM_H

#include "common/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* MCU/anim 通道映射 —— 根据项目实际定义 */
#define LED_CH_RED    0
#define LED_CH_BLUE   1
#define LED_SM_CH_MAX 2          /* 若再加灯，只需调这个宏 */

typedef enum
{
    LED_SM_OFF = 0,              /* 灭灯 */
    LED_SM_ON,					 /* 常亮 */
    LED_SM_BREATH,               /* 呼吸：period_ms 为完整往返 */
} led_sm_mode_t;

/* 初始化所有通道为 OFF */
void  led_sm_init(void);

/* 切换通道模式
 *  ch         : 0..LED_SM_CH_MAX-1
 *  mode       : LED_SM_xxx
 *  period_ms  : 对 BREATH/FLASH 的周期，单位 ms；OFF 模式忽略
 */
void  led_sm_set(u8 ch, led_sm_mode_t mode, u16 period_ms);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* SERVICES_BLUE_LED_SM_H */
