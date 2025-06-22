#ifndef LED_PORT_H
#define LED_PORT_H
#include "common/types.h"

/* 供 soft_pwm 调用的亮/灭操作（逻辑 LED 编号） */
void led_on (u8 id);
void led_off(u8 id);

#endif /* LED_PORT_H */
