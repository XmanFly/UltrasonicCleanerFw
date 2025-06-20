#ifndef __FSM_H__
#define __FSM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

void fsm_init(void);
void fsm_loop(void);

#ifdef PLATFORM_QT
/* -------- 事件回调钩子 -------- */
typedef void (*fsm_cb_tb)(u8 ctrl);
void fsm_set_callback(fsm_cb_tb cb);
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
