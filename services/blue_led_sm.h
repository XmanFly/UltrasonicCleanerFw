## inc/services/blue_led_sm.h
#ifndef SERVICES_BLUE_LED_SM_H
#define SERVICES_BLUE_LED_SM_H

#include <stdint.h>

/* Public animation presets for the blue LED */
typedef enum {
    BLUE_LED_OFF = 0,        /* 灯灭                           */
    BLUE_LED_BREATH_NORMAL,  /* 1.5 s 呼吸 – 默认工作指示        */
    BLUE_LED_BREATH_FAST     /* 100 ms 超快呼吸 / 快闪 – 低电量 */
} blue_led_mode_t;

void blue_led_sm_init(void);
void blue_led_sm_set(blue_led_mode_t mode);

#endif /* SERVICES_BLUE_LED_SM_H */