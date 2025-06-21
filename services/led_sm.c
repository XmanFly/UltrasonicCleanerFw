#include "services/anim.h"
#include "led_sm.h"

typedef struct {
    u8         mode;        /* 当前模式 LED_SM_xxx                */
    u16        period;      /* 当前周期 (ms) — 对 OFF 无意义       */
} led_ctx_t;

static led_ctx_t ctx[LED_SM_CH_MAX];

/* ----------------------------------------------------------- */
void led_sm_init(void)
{
	u8 i;
    for(i = 0; i < LED_SM_CH_MAX; ++i) {
        ctx[i].mode   = LED_SM_OFF;
        ctx[i].period = 0;
        anim_set(i, ANIM_NONE, 0);
    }
}

/* ----------------------------------------------------------- */
void led_sm_set(u8 ch, led_sm_mode_t mode, u16 period_ms)
{	
	led_ctx_t *c = &ctx[ch];
	
    if(ch >= LED_SM_CH_MAX) return;

    /* 若模式和周期均未改变 → 直接返回，避免多余调用 */
    if(mode == c->mode && (mode == LED_SM_OFF || period_ms == c->period))
        return;

    switch(mode)
    {
    case LED_SM_OFF:
        anim_set(ch, ANIM_NONE, 0);
        break;
    case LED_SM_BREATH:
        /* 若以前已是 BREATH，则用 update 无闪烁更改周期 */
        if(c->mode == LED_SM_BREATH)
            anim_update_period(ch, period_ms);
        else
            anim_set(ch, ANIM_BREATH, period_ms);
        break;
    case LED_SM_ON:
        anim_set(ch, ANIM_CONST, 0);
        break;
    }

    c->mode   = (u8)mode;
    c->period = period_ms;
}
