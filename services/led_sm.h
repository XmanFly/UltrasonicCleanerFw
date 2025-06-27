#ifndef LED_SM_H
#define LED_SM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"
#include "led_group.h"          /* 分组底层 */

/* -------- 组号保持旧名字 -------- */
#define LED_CH_RED     0
#define LED_CH_BLUE    1

/* -------- 几种呼吸频率参数 -------- */
#define BREATH_NORMAL    10
#define BREATH_FAST		 1

/* -------- 状态机支持的模式 -------- */
typedef enum {
    LS_MODE_OFF      = 0,
    LS_MODE_CONST    = 1,
    LS_MODE_BREATHE  = 2,
    LS_MODE_BLINK    = 3
} ls_mode_e;

/* ---------- API ---------- */
void led_sm_init(void);

void led_sm_off     (u8 ch);
void led_sm_const   (u8 ch, u8 pct0_100);            /* 恒亮 0-100 %  */
void led_sm_breathe (u8 ch, u8 speed_div);           /* 呼吸周期 分频系数 */
void led_sm_blink   (u8 ch, u8 pct0_100,
                     u16 on_ms, u16 off_ms);         /* 闪烁 */

void led_sm_tick(void);   /* 需在 2 ms 定时调度里调用 */

#ifdef PLATFORM_QT
typedef void (*led_sm_cb_tb)(u8 ch, u8 mode, u8 speed);
void led_sm_set_callback(led_sm_cb_tb cb);
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* LED_SM_H */
