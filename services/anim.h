#ifndef ANIM_H
#define ANIM_H
#include "common/types.h"

#define ANIM_CH_COUNT  2     /* 通道数：红灯、蓝灯 */
#define ANIM_TBL_SIZE  64    /* 半周期采样点数     */

typedef void (*anim_output_fn)(u8 id, u8 lv_0_15);

/* 绑定输出口（→ led_set_level） */
void anim_bind_output(anim_output_fn fn);

/* 每个通道单独配置 LED 及速度（1 = 128 ms；N = 128 ms×N） */
void anim_channel_cfg(u8 ch, u8 led_id, u8 speed_div);

/* 生命周期 */
void anim_init(void);
void anim_tick_2ms(void);    /* 需每 2 ms 调一次 */

#endif
