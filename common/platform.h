#ifndef __PLATFORM_CFG_H__
#define __PLATFORM_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PLATFORM_QT
    #include <stdio.h>

    #define INLINE static
    #define xdata
    #define code

    /* 任意格式化输出 + 立即刷新 */
    #define qtPrint(fmt, ...)                                        \
        do {														 \
            fprintf(stdout, (fmt), ##__VA_ARGS__);                   \
            fflush(stdout);                                          \
        } while (0)
#else
    #include "STC8H.h"
    void qtPrint(const char *fmt, ...);
#endif

#define C51_DEBUG_OUT

#ifdef __cplusplus
}   /* extern "C" */
#endif


#endif
