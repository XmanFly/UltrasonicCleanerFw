#ifndef POWER_H
#define POWER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"
	
void power_init();
	
/* 供电自锁 */
void power_on ();
void power_off();

#ifdef PLATFORM_QT
/* -------- 事件回调钩子 -------- */
typedef void (*power_cb_tb)(u8 id, u8 on);
void power_set_callback(led_cb_tb cb);
#endif


#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* POWER_H */
