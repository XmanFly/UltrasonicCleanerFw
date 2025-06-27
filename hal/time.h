#ifndef __HAL_TIME_H__
#define __HAL_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

void hal_time_init(void);
void hal_time_tick_1ms(void);

#ifdef PLATFORM_QT
void hal_time_tick();
#endif

// 主循环用的电池采样标志
extern volatile u8  batt_due;

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
