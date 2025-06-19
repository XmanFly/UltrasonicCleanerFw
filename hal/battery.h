#ifndef __HAL_BATT_H__
#define __HAL_BATT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"
void  hal_batt_init(void);
u16   hal_batt_get_mv(void);
u8    hal_batt_is_chg(void);

#ifdef PLATFORM_QT
void  hal_batt_sim_set_mv(u16 mv);
void  hal_batt_sim_set_chg(u8 on);
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
