#ifndef __TOUCH_SERVICE_H__
#define __TOUCH_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

/* semantic events delivered to upper layer */
typedef enum
{
    TOUCH_EVT_NONE = 0,
    TOUCH_EVT_EDGE,       /* one rising edge (press)            */
    TOUCH_EVT_PRESS_500,  /* reached 0.5 s threshold            */
    TOUCH_EVT_PRESS_2S    /* reached 2 s threshold              */
} touch_evt_t;

/* initialize internal state */
void touch_service_init(void);

/* 1-ms tick – must be called from hal_time_tick_1ms() */
void touch_service_tick_1ms(void);

/* poll one pending event; returns TOUCH_EVT_NONE if empty */
touch_evt_t touch_service_fetch_event(void);

/* current stable press duration (ms); 0 if key released */
u16 touch_service_press_ms(void);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* __TOUCH_SERVICE_H__ */
