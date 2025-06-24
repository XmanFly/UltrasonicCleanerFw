
#include "hal/ultrasonic.h"
#include "common/platform.h"

#ifndef PLATFORM_QT
#include "STC8H.h"
#endif

/* ====== 内部静态状态 ====== */
static volatile unsigned short us_curFreqKHz = HAL_US_F_MIN_KHZ;
static volatile unsigned char  us_dirUp      = 1u;     /* 1=向上扫,0=向下扫 */
static volatile unsigned short us_tickCnt    = 0u;
static volatile bit            us_running    = 0;

static volatile u16 arr;
static volatile u16 cmpVal;

/* ====== 计算常量 ====== */
#define US_STEP_MS    ( (HAL_US_HALF_SWEEP_MS) * (HAL_US_F_STEP_KHZ) / \
                       ( (HAL_US_F_MAX_KHZ) - (HAL_US_F_MIN_KHZ) ) ) /* 25 */
#define US_STEP_TICK  ( (US_STEP_MS) / (HAL_US_TICK_MS) )            /* 25 */

#define US_PWM_MODE1  0x68   /* OCxM=110, OCxPE=1 → PWM mode-1, 预装载 */

/* ====== 内部函数：设置频率 ====== */
static void us_setFreq(unsigned short kHz)
{
    volatile u16 kHz_backup = kHz;
    volatile u16 t2 = (u16)HAL_US_FOSC_KHZ / kHz_backup;
    volatile u16 arr = t2 -1;
    volatile u16 cmpVal = arr >> 1;                       /* 50 % 占空 */

    EA = 0;

    /* —— 按官方顺序：先比较后周期，且都高字节→低字节 —— */
    PWMA_CCR4H = cmpVal >> 8;
    PWMA_CCR4L = (u8)cmpVal;
    PWMA_ARRH  = arr >> 8;
    PWMA_ARRL  = (u8)arr;
    EA = 1;
}

/* ====== HAL 实现 ====== */
void hal_us_init(void)
{
    /* ---- P3.3 设为推挽输出 ---- */
    P3M0 |=  0x08;       /* M0=1 */
    P3M1 &= ~0x08;       /* M1=0 -> Push-pull */

    /* ---- 映射 PWM4N → P3.3 ---- */
    P_SW2  |= 0x80;                      /* 进入 XFR 区 */
    PWMA_PS = (PWMA_PS & ~0xC0) | 0xC0;  /* C4PS = 11 (P3.4 / P3.3) */

    /* ---- PWMA 通道 4N 基本配置 ---- */
    PWMA_ENO  |= 0x80;                   /* ENO4N =1             */
    PWMA_PSCRH = 0;  PWMA_PSCRL = 0;     /* 预分频 1             */
    PWMA_CCMR4 = US_PWM_MODE1;           /* PWM mode-1, 预装载    */
    PWMA_CCER2 = 0x40;                   /* **CC4NE=1** (N相使能) */
    PWMA_BKR   = 0x80;                   /* **MOE=1**  主输出使能 */
    PWMA_CR1   = 0x80;                   /* ARPE=1               */

    us_setFreq(HAL_US_F_MIN_KHZ);        /* 默认 138 kHz          */
}

void hal_us_start(void)
{
    us_curFreqKHz = HAL_US_F_MIN_KHZ;
    us_dirUp      = 1u;
    us_tickCnt    = 0u;
    us_running    = 1;    
    PWMA_CR1     |= 0x01;                         /* CEN=1 启动计数 */
    print("hal_us_start\n");
}

void hal_us_stop(void)
{
    PWMA_CR1     &= ~0x01;                        /* 关闭计数器     */
    us_running    = 0;
    print("hal_us_stop\n");
}

void hal_us_tick_1ms(void)
{
    if (!us_running) return;

    if (++us_tickCnt < US_STEP_TICK) return;
    us_tickCnt = 0u;

    /* ------ 计算下一频率 ------ */
    if (us_dirUp)
    {
        if (us_curFreqKHz < HAL_US_F_MAX_KHZ)
        {
            us_curFreqKHz += HAL_US_F_STEP_KHZ;
        }
        else
        {
            us_dirUp = 0u;
            us_curFreqKHz -= HAL_US_F_STEP_KHZ;
        }
    }
    else
    {
        if (us_curFreqKHz > HAL_US_F_MIN_KHZ)
        {
            us_curFreqKHz -= HAL_US_F_STEP_KHZ;
        }
        else
        {
            us_dirUp = 1u;
            us_curFreqKHz += HAL_US_F_STEP_KHZ;
        }
    }

    us_setFreq(us_curFreqKHz);
    // us_setFreq(HAL_US_F_MAX_KHZ);
    
    // print("hal_us_tick_1ms\n");

}
