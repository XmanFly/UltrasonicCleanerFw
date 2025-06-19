#ifndef __HAL_LED_H__
#define __HAL_LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

void hal_blue_pwm_set(u8 duty);
void hal_red_pwm_set(u8 idx,u8 duty);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
