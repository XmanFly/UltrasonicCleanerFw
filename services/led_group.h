#ifndef LED_GROUP_H
#define LED_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"
#include "common/platform.h"

/* ----------（可根据硬件修改）---------- */
/* 1) 组数 */
#define LED_GROUP_CNT          2           /* 红、蓝 */

/* 2) 每组最大灯数（所有组共用同一上限） */
#if !UART_ENABLE   
#define LED_RED_GROUP_CNT        9           /* 红灯组内灯个数 */
#else
#define LED_RED_GROUP_CNT        7
#endif

#define LED_BLUE_GROUP_CNT       1           /* 蓝灯组内灯个数 */

/* 3) 每只 LED 连接的物理 IO（线性编号 0..TOTAL-1）
 *    超出实际数量用 0xFF 占位。示例：红灯 0~9→P10~P19 ……
 */
#define LED_TOTAL   (LED_RED_GROUP_CNT + LED_BLUE_GROUP_CNT)
/* 每路 LED 对应一个端口地址和一个位掩码 */
typedef struct {
    u8 port_no;   /* 1 = P1, 2 = P2, … */
    u8           mask;   /* 对应位：1<<0、1<<1… */
} LedIo_t;	
#ifdef __C51__
extern data volatile const LedIo_t red_group[LED_RED_GROUP_CNT];
extern data volatile const LedIo_t blue_group[LED_BLUE_GROUP_CNT];
#else
extern volatile const LedIo_t red_group[LED_RED_GROUP_CNT];
extern volatile const LedIo_t blue_group[LED_BLUE_GROUP_CNT];
#endif

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

void led_group_tick(void);   /* 需在 2 ms 节拍里调用 */

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* LED_GROUP_H */
