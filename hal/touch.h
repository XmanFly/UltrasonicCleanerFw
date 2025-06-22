#ifndef __HAL_TOUCH_H__
#define __HAL_TOUCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

/* Initialize capacitive-touch key */
void     hal_touch_init(void);

/* Realtime key level (1 = currently pressed) */
u8  hal_touch_is_pressed(void);

#ifdef PLATFORM_QT       /* Qt simulation helper */
void hal_touch_sim_press(void);      /* generate press edge + level = 1  */
void hal_touch_sim_release(void);    /* generate release edge + level = 0 */
void hal_touch_sim_set(u8 level);   /* 0 = up , 1 = down (edge auto) */
#endif

#ifdef __cplusplus
}
#endif
#endif   /* __HAL_TOUCH_H__ */
