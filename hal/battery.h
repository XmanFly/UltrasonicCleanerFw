#ifndef __HAL_BATT_H__
#define __HAL_BATT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

/* ----------- 可调宏区 ----------- */
#ifndef BAT_SAMPLE_NUM
#define BAT_SAMPLE_NUM   4u      /* 每个通道平均次数(1~32), 越大越稳但越慢 */
#endif
#ifndef BAT_R1_KOHM
#define BAT_R1_KOHM     100u     /* 电池+ → P3.6 上拉电阻(kΩ) */
#endif
#ifndef BAT_R2_KOHM
#define BAT_R2_KOHM      33u     /* P3.6 → GND 下拉电阻(kΩ) */
#endif
/* -------------------------------- */

void  hal_battery_init(void);
void  hal_battery_task(void);            /* 阻塞式, 请在主循环调用 */
u16   hal_battery_get_mv(void);          /* 返回最近一次毫伏值 */
u8    hal_battery_get_percent(void);     /* 简单 3.3-4.2 V 线性映射 */
u8    hal_battery_is_chg(void);			 /* 电池充电中 */

#ifdef PLATFORM_QT
void  hal_battery_sim_set_mv(u16 mv);
void  hal_battery_sim_set_chg(u8 on);
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
