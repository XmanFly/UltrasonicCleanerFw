#include "led_sm.h"

/* ---------------- 内部状态 ---------------- */
typedef struct {
    u8  mode;      /* ls_mode_e */
    u8  pct;       /* CONST / BLINK 亮度 % */
    u8  div;       /* 呼吸分频             */
    u16 on_tick;   /* BLINK 亮持续 (2 ms)  */
    u16 off_tick;  /* BLINK 灭持续 (2 ms)  */
    u16 cnt;
    u8 blink_on;
} sm_t;

static sm_t s[LED_GROUP_CNT];

/* ---------- 私有：把状态写到底层 ---------- */
static void apply(u8 ch)
{
    switch (s[ch].mode) {
    case LS_MODE_OFF:
        led_group_set_mode(ch, LG_MODE_OFF);
        break;
    case LS_MODE_CONST:
        led_group_set_const(ch, s[ch].pct);
        break;
    case LS_MODE_BREATHE:
        led_group_set_breathe(ch, s[ch].div);
        break;
    case LS_MODE_BLINK:
        if (s[ch].blink_on) led_group_set_const(ch, s[ch].pct);
        else                led_group_set_mode (ch, LG_MODE_OFF);
        break;
    }
}

/* ---------- 公共 API ---------- */
void led_sm_init(void)
{
    u8 i;
    led_group_init();                     /* 底层初始化 */

    for (i = 0; i < LED_GROUP_CNT; ++i) {
        s[i].mode = LS_MODE_OFF;
        s[i].pct  = 0;
        s[i].div  = 1;
        s[i].on_tick  = 0;
        s[i].off_tick = 0;
        s[i].cnt      = 0;
        s[i].blink_on = 0;
        apply(i);
    }
}

void led_sm_off(u8 ch)
{
    if (ch >= LED_GROUP_CNT) return;
    s[ch].mode = LS_MODE_OFF;
    apply(ch);
}

void led_sm_const(u8 ch, u8 pct)
{
    if (ch >= LED_GROUP_CNT) return;
    if (pct > 100) pct = 100;
    s[ch].mode = LS_MODE_CONST;
    s[ch].pct  = pct;
    apply(ch);
}

void led_sm_breathe(u8 ch, u8 div)
{
    if (ch >= LED_GROUP_CNT || div == 0) return;
    s[ch].mode = LS_MODE_BREATHE;
    s[ch].div  = div;
    if (s[ch].pct == 0) s[ch].pct = 100;      /* 默认峰值 */
    apply(ch);
}

void led_sm_blink(u8 ch, u8 pct, u16 on_ms, u16 off_ms)
{
    if (ch >= LED_GROUP_CNT || on_ms == 0 || off_ms == 0) return;
    if (pct > 100) pct = 100;

    s[ch].mode      = LS_MODE_BLINK;
    s[ch].pct       = pct;
    s[ch].on_tick   = on_ms  / 2;   /* 2 ms 一个 tick */
    s[ch].off_tick  = off_ms / 2;
    s[ch].cnt       = s[ch].on_tick;
    s[ch].blink_on  = 1;
    apply(ch);
}

void led_sm_tick_2ms(void)
{
    u8 i;
    for (i = 0; i < LED_GROUP_CNT; ++i) {
        if (s[i].mode == LS_MODE_BLINK) {
            if (--s[i].cnt == 0) {
                s[i].blink_on = !s[i].blink_on;
                s[i].cnt = s[i].blink_on ? s[i].on_tick
                                         : s[i].off_tick;
                apply(i);
            }
        }
    }
    led_group_tick_2ms();    /* 更新呼吸曲线 */
}
