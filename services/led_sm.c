#include "led_sm.h"
#include "common/platform.h"

#ifdef PLATFORM_QT
static led_sm_cb_tb user_cb = 0;
#endif

/* ---------------- 内部状态 ---------------- */
typedef struct {
    u8  mode;      /* ls_mode_e 当前模式 */
    u8  pct;       /* CONST / BLINK 亮度 % */
    u8  div;       /* 呼吸分频             */
    u16 on_tick;   /* BLINK 亮持续 (2 ms)  */
    u16 off_tick;  /* BLINK 灭持续 (2 ms)  */
    u16 cnt;       /* BLINK 计数           */
    u8  blink_on;  /* 1=当前亮,0=灭        */
} sm_t;

static volatile sm_t s[LED_GROUP_CNT];

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
        if (s[ch].blink_on) {
            led_group_set_const(ch, s[ch].pct);
        } else {
            led_group_set_mode(ch, LG_MODE_OFF);
        }
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
    if (s[ch].mode == LS_MODE_OFF) return;     /* 状态未变，不做动作 */

    s[ch].mode = LS_MODE_OFF;
    apply(ch);

#ifdef PLATFORM_QT
    if (user_cb) {
        user_cb(ch, LS_MODE_OFF, 0);
    }
#endif
}

void led_sm_const(u8 ch, u8 pct)
{
    if (ch >= LED_GROUP_CNT) return;
    if (pct > 100) pct = 100;

    /* 状态机保护：仅当模式或参数变化时才更新底层 */
    if (s[ch].mode == LS_MODE_CONST && s[ch].pct == pct) return;

    s[ch].mode = LS_MODE_CONST;
    s[ch].pct  = pct;
    apply(ch);

    print("led_sm_const\n");

#ifdef PLATFORM_QT
    if (user_cb) {
        user_cb(ch, LS_MODE_CONST, 100);
    }
#endif
}

void led_sm_breathe(u8 ch, u8 div)
{
    if (ch >= LED_GROUP_CNT || div == 0) return;

    /* 与之前状态比较 */
    if (s[ch].mode == LS_MODE_BREATHE && s[ch].div == div) return;

    s[ch].mode = LS_MODE_BREATHE;
    s[ch].div  = div;
    if (s[ch].pct == 0) s[ch].pct = 100;      /* 默认峰值 */
    apply(ch);

    print("led_sm_breathe ch %bu div %bu\n", ch, div);

#ifdef PLATFORM_QT
    if (user_cb) {
        user_cb(ch, LS_MODE_BREATHE, div);
    }
#endif
}

void led_sm_blink(u8 ch, u8 pct, u16 on_ms, u16 off_ms)
{
    u16 on_tick, off_tick;
    if (ch >= LED_GROUP_CNT || on_ms == 0 || off_ms == 0) return;
    if (pct > 100) pct = 100;

    on_tick  = on_ms  / 2;   /* 2 ms 一个 tick */
    off_tick = off_ms / 2;

    /* 如果模式与所有关键参数均相同，则不需重新配置底层 */
    if (s[ch].mode == LS_MODE_BLINK &&
        s[ch].pct  == pct &&
        s[ch].on_tick  == on_tick &&
        s[ch].off_tick == off_tick) {
        return;
    }

    s[ch].mode      = LS_MODE_BLINK;
    s[ch].pct       = pct;
    s[ch].on_tick   = on_tick;
    s[ch].off_tick  = off_tick;
    s[ch].cnt       = s[ch].on_tick;
    s[ch].blink_on  = 1;
    apply(ch);

    print("led_sm_blink\n");

}

void led_sm_tick(void)
{
    volatile u8 i;
    for (i = 0; i < LED_GROUP_CNT; ++i) {
        if (s[i].mode == LS_MODE_BLINK) {
            if (--s[i].cnt == 0) {
                s[i].blink_on = !s[i].blink_on;
                s[i].cnt = s[i].blink_on ? s[i].on_tick : s[i].off_tick;
                apply(i);
            }
        }
    }
    led_group_tick();    /* 更新呼吸曲线 */
}

#ifdef PLATFORM_QT
void led_sm_set_callback(led_sm_cb_tb cb)
{
    user_cb = cb;
}
#endif
