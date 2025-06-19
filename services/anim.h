#ifndef __ANIM_H__
#define __ANIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

typedef enum {
    ANIM_NONE,
    ANIM_BREATH,
    ANIM_FLASH
} anim_t;

void anim_init(void);
void anim_set(anim_t t,u16 period_ms,u16 on_ms);
void anim_tick_2ms(void);
u8   anim_get_level(void);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
