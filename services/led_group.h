#ifndef LED_GROUP_H
#define LED_GROUP_H
#include "common/types.h"

/* ----------（可根据硬件修改）---------- */
/* 1) 组数 */
#define LED_GROUP_CNT          3           /* 红、蓝、绿 */

/* 2) 每组最大灯数（所有组共用同一上限） */
#define LED_PER_GROUP_MAX     10

/* 3) 每只 LED 连接的物理 IO（线性编号 0..TOTAL-1）
 *    超出实际数量用 0xFF 占位。示例：红灯 0~9→P10~P19 ……
 */
#define LED_TOTAL   (LED_GROUP_CNT * LED_PER_GROUP_MAX)
extern const u8 led_io_map[LED_TOTAL];

/* 4) 各组实际灯数 */
extern const u8 led_group_size[LED_GROUP_CNT];
/* ------------------------------------ */

/* 组工作模式 */
typedef enum {
    LG_MODE_OFF     = 0,  /* 全灭          */
    LG_MODE_CONST   = 1,  /* 恒亮          */
    LG_MODE_BREATHE = 2   /* 呼吸          */
} lg_mode_e;

/* ========= API ========= */
void led_group_init(void);

void led_group_set_mode   (u8 grp, lg_mode_e m);      /* 只改模式 */
void led_group_set_const  (u8 grp, u8 pct0_100);      /* 恒亮 %  */
void led_group_set_breathe(u8 grp, u8 speed_div);     /* 1=128 ms */

void led_group_tick_2ms(void);   /* 需在 2 ms 节拍里调用 */

#endif /* LED_GROUP_H */
