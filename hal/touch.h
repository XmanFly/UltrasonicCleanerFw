#ifndef __HAL_TOUCH_H__
#define __HAL_TOUCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

void hal_touch_init(void);
u8   hal_touch_event(void);

#ifdef PLATFORM_QT
void hal_touch_sim_trigger(void);
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
