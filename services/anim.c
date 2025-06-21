#include "anim.h"
#include <string.h>

#ifdef PLATFORM_QT
static anim_cb_tb user_cb = 0;
void anim_set_callback(anim_cb_tb cb)
{
    user_cb = cb;
}
#endif

#define LUT_LEN 128   /* 128 points → 7‑bit */

/* 半余弦查表 (0…π)
 *  python:  round(sin(i*pi/127))*127.5) for i in 0..127
 */
static const u8 cos_lut[LUT_LEN] = {
    0,   3,   6,   9,  13,  16,  19,  22,  25,  28,  31,  34,  37,  40,  43,  46,
    49,  52,  55,  58,  61,  63,  66,  69,  71,  74,  76,  79,  81,  84,  86,  88,
    91,  93,  95,  97,  99, 101, 103, 105, 107, 108, 110, 111, 113, 114, 116, 117,
    118, 119, 120, 121, 122, 123, 124, 125, 125, 126, 126, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 126, 126, 125, 125, 124, 123, 122, 121, 120, 119, 118,
    117, 116, 114, 113, 111, 110, 108, 107, 105, 103, 101,  99,  97,  95,  93,  91,
    88,  86,  84,  81,  79,  76,  74,  71,  69,  66,  63,  61,  58,  55,  52,  49,
    46,  43,  40,  37,  34,  31,  28,  25,  22,  19,  16,  13,   9,   6,   3,   0
};

typedef struct {
    anim_type_t type;
    u16    period;   /* ms / 完整呼吸周期 */
    u16    phase;    /* 0…period */
} anim_ch_t;

static anim_ch_t ch[ANIM_CH_MAX];

void anim_init(void)
{
    memset(ch, 0, sizeof(ch));
}

void anim_set(u8 id, anim_type_t type, u16 period_ms)
{
    if (id >= ANIM_CH_MAX) return;
    ch[id].type   = type;
    ch[id].period = period_ms;
    ch[id].phase  = 0;
#ifdef PLATFORM_QT
    user_cb(id, type, period_ms);
#endif
}

/* 在不闪跳的前提下动态调周期 */
void anim_update_period(u8 id, u16 new_per)
{
    if (id >= ANIM_CH_MAX) return;
    if (ch[id].type == ANIM_NONE) return;
    ch[id].phase = (u32)ch[id].phase * new_per / ch[id].period;
    ch[id].period = new_per;
}

static u8 lut_sample(u16 phase, u16 period)
{
    /* 将相位按 0‑127 缩放 */
    u32 idx = ((u32)phase * LUT_LEN) / period;
    return cos_lut[idx & 0x7F];
}

u8 anim_get_level(u8 id)
{
    if (id >= ANIM_CH_MAX) return 0;

    anim_ch_t *a = &ch[id];
    switch(a->type) {
    case ANIM_BREATH:   return lut_sample(a->phase, a->period);
    default:            return 0;
    }
}

/* 系统 1 ms tick 里每 2 ms 调用一次 */
void anim_tick_2ms(void)
{
    for (u8 i = 0; i < ANIM_CH_MAX; ++i) {
        anim_ch_t *a = &ch[i];
        if (a->type == ANIM_NONE) continue;
        a->phase += 2;                       /* 2 ms 步进 */
        if (a->phase >= a->period) {
            a->phase = 0;
        }
    }
}

