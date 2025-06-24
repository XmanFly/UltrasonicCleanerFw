#ifndef _HAL_US_H_
#define _HAL_US_H_

/* ---------- 配置宏 ---------- */
#define HAL_US_FOSC_HZ          11059200UL   /* 晶振频率 Hz (11.0592 MHz) */
#define HAL_US_F_MIN_KHZ        138u         /* 扫频下限 kHz              */
#define HAL_US_F_MAX_KHZ        158u         /* 扫频上限 kHz              */
#define HAL_US_F_STEP_KHZ       1u           /* 每步增减 kHz              */
#define HAL_US_HALF_SWEEP_MS    500u         /* 半程时间 ms (0.5 s)       */
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

#endif
