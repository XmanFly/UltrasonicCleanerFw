#ifndef __PLATFORM_CFG_H__
#define __PLATFORM_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RELEASE // 发布时启用 注意不能与下面两个宏同时启用
// #define UART_ENABLE // 注意串口使能时，由于100us定时器1被串口占用，
//                     // 导致pwm任务soft_pwm_tick只能放在1ms定时器中执行，导致呼吸灯节奏异常
// #define PRINT

#ifdef PLATFORM_QT
    #include <stdio.h>

    #define INLINE static
    #define xdata
    #define code

    /* 任意格式化输出 + 立即刷新 */
    #define print(fmt, ...)                                        \
        do {														 \
            fprintf(stdout, (fmt), ##__VA_ARGS__);                   \
            fflush(stdout);                                          \
        } while (0)
#else
    #include "STC8H.h"
    void print(const char *fmt, ...);
#endif

#define C51_DEBUG_OUT

#ifdef __cplusplus
}   /* extern "C" */
#endif


#endif
