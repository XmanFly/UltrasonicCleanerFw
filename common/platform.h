#ifndef __PLATFORM_CFG_H__
#define __PLATFORM_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PLATFORM_QT
    #include <stdint.h>
    #define INLINE static
    #define xdata
#else
    #include "STC8H.h"
    #define INLINE static
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
