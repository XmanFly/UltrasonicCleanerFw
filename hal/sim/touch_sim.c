#include "hal/touch.h"
#ifdef PLATFORM_QT
#include <stdint.h>

/* edge event flag (one-shot) */
static u8 event_flag = 0;
/* debounced key level: 1 = pressed */
static u8 cur_state  = 0;

/* ------- public helpers ------- */
void hal_touch_sim_press(void)        { hal_touch_sim_set(1); }
void hal_touch_sim_release(void)      { hal_touch_sim_set(0); }

/* legacy alias */
void hal_touch_sim_trigger(void)      { hal_touch_sim_press(); }

/* generic setter: generates edge when level toggles */
void hal_touch_sim_set(u8 level)
{
    level = level ? 1 : 0;
    if(level != cur_state)            /* edge detected             */
        event_flag = 1;
    cur_state = level;
}

/* ------ HAL API ------ */

void hal_touch_init(void)
{
    event_flag = 0;
    cur_state  = 0;
}

u8 hal_touch_event(void)
{
    u8 e = event_flag;
    event_flag = 0;                   /* clear after read          */
    return e;
}

u8 hal_touch_is_pressed(void)
{
    return cur_state;
}
#endif   /* PLATFORM_QT */
