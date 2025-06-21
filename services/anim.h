// ========================================================
// 1.  inc/services/anim.h
// ========================================================
#ifndef SERVICES_ANIM_H
#define SERVICES_ANIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "common/types.h"

#define LED_RED   0         /* 9  ×  红灯  同步通道  */
#define LED_BLUE  1         /* 1  ×  蓝灯  独立通道  */
#define ANIM_CH_MAX 2

typedef enum { ANIM_NONE = 0, ANIM_BREATH, ANIM_CONST } anim_type_t;

void     anim_init(void);
void     anim_set(u8 ch, anim_type_t type, u16 period_ms);
u8  anim_get_level(u8 ch);          /* 0‑127 */
void     anim_tick_2ms(void);                 /* 调用周期: 2 ms */
/* 可选: 运行时改变周期 / 相位不闪跳 */
void     anim_update_period(u8 ch, u16 new_period_ms);

#ifdef PLATFORM_QT       /* Qt simulation helper */
/* -------- 事件回调钩子 -------- */
typedef void (*anim_cb_tb)(u8 ch, u8 type, u16 period);
void anim_set_callback(anim_cb_tb cb);
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* SERVICES_ANIM_H */
