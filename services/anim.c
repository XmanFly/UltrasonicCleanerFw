#include "anim.h"

/* √4-bit 半正弦 + γ≈sqrt  LUT */
static const u8 tbl[ANIM_TBL_SIZE] = {
      0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,
      7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,
     15,15,15,15,15,14,14,13,13,12,12,11,11,10,10,9,
      9,8,8,7,7,6,6,5,5,4,4,3,3,2,2,1
};

typedef struct {
    u8 led;       /* 绑定软 PWM 的 LED ID */
    u8 div;       /* 分频系数 ≥1          */
    u8 cnt;       /* 分频计数             */
    u8 idx;       /* LUT 索引 0-63        */
} chan_t;

static chan_t        ch[ANIM_CH_COUNT];
static anim_output_fn out_cb = 0;

void anim_bind_output(anim_output_fn fn)   { out_cb = fn; }

void anim_channel_cfg(u8 n, u8 led_id, u8 div)
{
    if (n >= ANIM_CH_COUNT || div == 0) return;
    ch[n].led = led_id;  ch[n].div = div;
}

void anim_init(void)
{
	u8 i;
    for (i = 0; i < ANIM_CH_COUNT; ++i) {
        ch[i].led = i;
        ch[i].div = 1;
        ch[i].cnt = ch[i].idx = 0;
    }
}

/* 2 ms 调度 */
void anim_tick_2ms(void)
{
	u8 i;
    if (!out_cb) return;
    for (i = 0; i < ANIM_CH_COUNT; ++i) {
        chan_t *c = &ch[i];
        if (++c->cnt >= c->div) {         /* 到达分频 */
            c->cnt = 0;
            if (++c->idx >= ANIM_TBL_SIZE) c->idx = 0;
        }
        out_cb(c->led, tbl[c->idx]);      /* 0-15 → soft_pwm */
    }
}
