#ifndef __HAL_BATT_H__
#define __HAL_BATT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

/* ----------- 可调宏区 ----------- */
#define BAT_SAMPLE_NUM   8u      /* 每个通道平均次数(1~32), 越大越稳但越慢 */
#define BAT_R1_KOHM     51u     /* 电池+ → P3.6 上拉电阻(kΩ) */
#define BAT_R2_KOHM      51u     /* P3.6 → GND 下拉电阻(kΩ) */
#define BAT_VOL_RATIO    2u     /* (BAT_R1_KOHM + BAT_R2_KOHM) / BAT_R2_KOHM */

/* -------------------------------- */

void  hal_battery_init(void);
void  hal_battery_task(void);            /* 阻塞式, 请在主循环调用 */
u16   hal_battery_get_mv(void);          /* 返回最近一次毫伏值 */
u8    hal_battery_get_percent(void);     /* 简单 3.3-4.2 V 线性映射 */
void  hal_battery_set_port_input();			 /* 设置电池充电引脚输入状态 */
void  hal_battery_set_port_output();			 /* 设置电池充电引脚输出状态 */
u8    hal_battery_is_chg(void);			 /* 电池充电中 */
void  hal_battery_set_chg(u8 on);			 /* 设置电池充电状态 */

#ifdef PLATFORM_QT
void  hal_battery_sim_set_mv(u16 mv);
void  hal_battery_sim_set_chg(u8 on);
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
