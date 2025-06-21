#ifndef __HAL_LED_H__
#define __HAL_LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

void led_hw_init(void);   /* GPIO + Timer‑3 reload‑table 准备 */
void led_hw_start(void);  /* 启动 Timer‑3 软‑PWM */

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
