#ifndef _HAL_US_H_
#define _HAL_US_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

/* ---------- 配置宏 ---------- */
#define HAL_US_FOSC_HZ          11059200UL   /* 晶振频率 Hz (11.0592 MHz) */
#define HAL_US_FOSC_KHZ         11059U   /* 晶振频率 Hz (11.0592 MHz) */
#define HAL_US_F_MIN_KHZ        120u         /* 扫频下限 kHz              */
#define HAL_US_F_MAX_KHZ        140u         /* 扫频上限 kHz              */
#define HAL_US_F_STEP_KHZ       2u           /* 每步增减 kHz              */
#define HAL_US_HALF_SWEEP_MS    10u         /* 半程时间 ms (0.5 s)       */
#define HAL_US_TICK_MS          1u           /* 系统 Tick 周期 ms         */
/* ---------------------------- */

/* ========= HAL 接口 ========= */
/* 系统上电时初始化硬件 PWM（P3.3） */
void hal_us_init(void);

/* 开始 138 k→158 k→138 k 往复扫频 */
void hal_us_start(void);

/* 停止 PWM 输出 */
void hal_us_stop(void);

/* 在现有 1 ms Tick 中断里调用，负责推进扫频 */
void hal_us_tick_1ms(void);

#ifdef PLATFORM_QT
u8 sim_us_on(void);
typedef void (*hal_us_cb_tb)(u8 on);
void hal_us_set_callback(hal_us_cb_tb cb);
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
