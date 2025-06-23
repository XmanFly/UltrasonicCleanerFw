#ifndef __SOFT_TIMER_H__
#define __SOFT_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

typedef void (*timer_cb_t)(void);

void timer_int();
int timer_start(u32 ms,timer_cb_t cb,u8 rep);
void timer_stop(int id);
void soft_timer_tick_1ms(void);
void soft_timer_task(void);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
