
#include "hal/ultrasonic.h"

#ifndef PLATFORM_QT
#include "STC8H.h"
#endif

/* ====== 内部静态状态 ====== */
static volatile unsigned short us_curFreqKHz = HAL_US_F_MIN_KHZ;
static volatile unsigned char  us_dirUp      = 1u;     /* 1=向上扫,0=向下扫 */
static volatile unsigned short us_tickCnt    = 0u;
static volatile bit            us_running    = 0;

/* ====== 计算常量 ====== */
#define US_STEP_MS    ( (HAL_US_HALF_SWEEP_MS) * (HAL_US_F_STEP_KHZ) / \
                       ( (HAL_US_F_MAX_KHZ) - (HAL_US_F_MIN_KHZ) ) ) /* 25 */
#define US_STEP_TICK  ( (US_STEP_MS) / (HAL_US_TICK_MS) )            /* 25 */

#define US_PWM_MODE1  0x68   /* OCxM=110, OCxPE=1 → PWM mode-1, 预装载 */

/* ====== 内部函数：设置频率 ====== */
static void us_setFreq(unsigned short kHz)
{
    unsigned short arr    = (unsigned short)((HAL_US_FOSC_HZ / 1000UL) / kHz) - 1u;
    unsigned short cmpVal = 0u;

    PWMA_ARRH  = (unsigned char)(arr >> 8);
    PWMA_ARRL  = (unsigned char) arr;

    cmpVal     = arr >> 1;                       /* 50 % 占空 */
    PWMA_CCR4H = (unsigned char)(cmpVal >> 8);
    PWMA_CCR4L = (unsigned char) cmpVal;

    PWMA_EGR   = 0x01;                           /* UG=1 → 更新影子寄存器 */
}

/* ====== HAL 实现 ====== */
void hal_us_init(void)
{
    /* ---- P3.3 推挽输出 (PWM4N_4) ---- */
    P3M0 |=  (1u << 3);
    P3M1 &= ~(1u << 3);

    /* ---- 映射 PWM4N 到 P3.3 ---- */
    P_SW2  |= 0x80;                               /* 进入 XFR 区 */
    PWMA_PS = (PWMA_PS & ~0xC0) | 0xC0;           /* C4PS = 11 */

    PWMA_ENO  |= 0x80;                            /* 使能 ENO4N     */
    PWMA_PSCRH = 0u; PWMA_PSCRL = 0u;             /* 预分频 1       */
    PWMA_CCMR4 = US_PWM_MODE1;                   /* PWM mode-1      */
    PWMA_CCER2 = 0x20;                            /* CC4E=1, 高电平 */
    PWMA_CR1   = 0x80;                            /* ARPE=1         */

    us_setFreq(HAL_US_F_MIN_KHZ);                 /* 默认 138 kHz   */
}

void hal_us_start(void)
{
    us_curFreqKHz = HAL_US_F_MIN_KHZ;
    us_dirUp      = 1u;
    us_tickCnt    = 0u;
    us_running    = 1;
    PWMA_CR1     |= 0x01;                         /* CEN=1 启动计数 */
}

void hal_us_stop(void)
{
    PWMA_CR1     &= ~0x01;                        /* 关闭计数器     */
    us_running    = 0;
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
}
