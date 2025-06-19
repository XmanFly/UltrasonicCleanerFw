#ifndef __HAL_US_H__
#define __HAL_US_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

void hal_us_start(void); // 开启超声波
void hal_us_stop(void); // 关闭超声波
	
#ifdef PLATFORM_QT
/* -------- 事件回调钩子 -------- */
typedef void (*hal_us_cb_tb)(u8 ctrl);
void hal_us_set_callback(hal_us_cb_tb cb);
#endif
	

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
