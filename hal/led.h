#ifndef LED_PORT_H
#define LED_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"
	
void led_init();
	
/* 供 soft_pwm 调用的亮/灭操作（逻辑 LED 编号） */
void led_on (u8 grp);
void led_off(u8 grp);

#ifdef PLATFORM_QT
/* -------- 事件回调钩子 -------- */
typedef void (*led_cb_tb)(u8 id, u8 on);
void led_set_callback(led_cb_tb cb);
#endif


#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* LED_PORT_H */
