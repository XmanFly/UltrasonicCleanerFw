// fac6609a-b7c0-40c1-9cfe-7f9f5035e8cc_lookup.c
#include "hal/ultrasonic.h"
#include "common/platform.h"

#ifndef PLATFORM_QT
#include "STC8H.h"
#endif

/* ====== 内部静态状态 ====== */
static volatile u16 us_curFreqKHz = HAL_US_F_MIN_KHZ;  /* 当前频率 kHz */
static volatile u8  us_dirUp      = 1u;               /* 1=向上扫,0=向下扫 */
static volatile u16 us_tickCnt    = 0u;               /* 滤波计数 */
static volatile bit us_running    = 0;                /* 是否在扫频 */


/* ====== 查表常量 ====== */
/* 表长度 = (MAX−MIN)/STEP + 1 = (158−138)/1 + 1 = 21 */
#define US_LUT_SIZE  ((HAL_US_F_MAX_KHZ - HAL_US_F_MIN_KHZ) / HAL_US_F_STEP_KHZ + 1)

/* 预先计算：ARR = floor(HAL_US_FOSC_KHZ / freq_kHz) − 1 */
static const u16 us_arr_tab[US_LUT_SIZE] = {
    /*120*/ 91, /*122*/ 89, /*124*/ 88, /*126*/ 86, /*128*/ 85,
    /*130*/ 84, /*132*/ 82, /*134*/ 81, /*136*/ 80, /*138*/ 79,
    /*140*/ 78
};
/* 计算方式：CCR = (D/100) × (ARR+1) 四舍五入取整 */
static const u16 us_duty_tab[US_LUT_SIZE] = {
    0x0025,  // 120kHz: CCR = 37 (0x0025)
    0x0022,  // 122kHz: CCR = 34 (0x0022)
    0x0020,  // 124kHz: CCR = 32 (0x0020)
    0x001E,  // 126kHz: CCR = 30 (0x001E)
    0x001B,  // 128kHz: CCR = 27 (0x001B)
    0x0019,  // 130kHz: CCR = 25 (0x0019)
    0x0017,  // 132kHz: CCR = 23 (0x0017)
    0x0015,  // 134kHz: CCR = 21 (0x0015)
    0x0013,  // 136kHz: CCR = 19 (0x0013)
    0x0012,  // 138kHz: CCR = 18 (0x0012)
    0x0010   // 140kHz: CCR = 16 (0x0010)
};

/* ====== 计算常量 ====== */
#define US_STEP_MS    ( (HAL_US_HALF_SWEEP_MS) * (HAL_US_F_STEP_KHZ) / \
                       ( (HAL_US_F_MAX_KHZ) - (HAL_US_F_MIN_KHZ) ) ) /* 25 */
#define US_STEP_TICK  ( (US_STEP_MS) / (HAL_US_TICK_MS) )            /* 25 */

#define US_PWM_MODE1  0x68   /* OCxM=110, OCxPE=1 → PWM mode-1, 预装载 */

/* ====== 内部函数：设置频率 ====== */
static void us_setFreq(u16 kHz)
{
    /* 计算在表中的索引，完全去掉除法 */
    volatile u8 idx = (u8)(kHz - HAL_US_F_MIN_KHZ) >> 1;

    /* 查表取出 ARR，再右移一次代替 50% 占空计算 */
    volatile u16 a = us_arr_tab[idx];
    volatile u16 duty = us_duty_tab[idx];  /* 高8位:CCR_H, 低8位:CCR_L */

    // EA = 0;  /* 关中断，按官方顺序先写 CCR 再写 ARR */

    PWMA_CCR4H = (u8)(duty >> 8);
    PWMA_CCR4L = (u8)(duty     );
    PWMA_ARRH  = (u8)(a >> 8);
    PWMA_ARRL  = (u8)(a      );
    
    // EA = 1;
}

/* ====== HAL 实现 ====== */
void hal_us_init(void)
{
    /* ---- P3.3 设为推挽输出 ---- */
    P3M0 |=  0x08;  /* M0=1 */
    P3M1 &= ~0x08;  /* M1=0 -> Push-pull */

    /* ---- 映射 PWM4N → P3.3 ---- */
    P_SW2  |= 0x80;                     /* 进入 XFR 区 */
    PWMA_PS = (PWMA_PS & ~0xC0) | 0xC0; /* C4PS = 11 (P3.4 / P3.3) */

    /* ---- PWMA 通道 4N 基本配置 ---- */
    PWMA_ENO   |= 0x80;   /* ENO4N =1 */
    PWMA_PSCRH  = 0;      /* 预分频 1 */
    PWMA_PSCRL  = 0;
    PWMA_CCMR4  = US_PWM_MODE1;  /* PWM mode-1, 预装载 */
    PWMA_CCER2  = 0x40;   /* CC4NE=1 (N相使能) */
    PWMA_BKR    = 0x80;   /* MOE=1  主输出使能 */
    PWMA_CR1    = 0x80;   /* ARPE=1 */

    us_setFreq(HAL_US_F_MIN_KHZ);  /* 默认 138 kHz */
}

void hal_us_start(void)
{
    us_curFreqKHz = HAL_US_F_MIN_KHZ;
    us_dirUp      = 1u;
    us_tickCnt    = 0u;
    us_running    = 1;
    PWMA_CR1     |= 0x01;  /* CEN=1 启动计数 */
    print("hal_us_start\n");
}

void hal_us_stop(void)
{
    /* 1) 先把占空比设 0（写 shadow） */
    PWMA_CCR4H = 0;
    PWMA_CCR4L = 0;

    /* 2) 触发一次软件更新，让 CCR 立即生效 */
    PWMA_EGR  = 0x01;       /* UG=1 */

    PWMA_CR1     &= ~0x01;  /* 关闭计数器 */
    P33 = 0;
    us_running    = 0;
    print("hal_us_stop\n");
}

void hal_us_tick_1ms(void)
{
    if (!us_running) return;

    if (++us_tickCnt < US_STEP_TICK) return;
    us_tickCnt = 0u;

    /* ------ 计算下一频率 ------ */
    if (us_dirUp) {
        if (us_curFreqKHz < HAL_US_F_MAX_KHZ) {
            us_curFreqKHz += HAL_US_F_STEP_KHZ;
        } else {
            us_dirUp      = 0u;
            us_curFreqKHz -= HAL_US_F_STEP_KHZ;
        }
    } else {
        if (us_curFreqKHz > HAL_US_F_MIN_KHZ) {
            us_curFreqKHz -= HAL_US_F_STEP_KHZ;
        } else {
            us_dirUp      = 1u;
            us_curFreqKHz += HAL_US_F_STEP_KHZ;
        }
    }

    us_setFreq(us_curFreqKHz);
}
