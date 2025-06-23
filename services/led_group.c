#include "led_group.h"
#include "soft_pwm.h"

/* √4-bit 半正弦 + γ≈sqrt  LUT */
static const u8 lut[64] = {
      0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,
      7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,
     15,15,15,15,15,14,14,13,13,12,12,11,11,10,10,9,
      9,8,8,7,7,6,6,5,5,4,4,3,3,2,2,1
};

/* ------- 默认 IO & 数量表（可改硬件后一起修改） ------- */
const u8 led_io_map[LED_TOTAL] = {
    /* 组0-红 */  0,1,2,3,4,5,6,7,8,9,
    /* 组1-蓝 */ 10,11,12,13,14,15,16,17,18,19,
    /* 组2-绿 */ 20,21,22,23,24,25,26,27,28,29
};
const u8 led_group_size[LED_GROUP_CNT] = {10,10,10};
/* ---------------------------------------------------- */

typedef struct {
    lg_mode_e mode;          /* lg_mode_e */
    u8 pct;           /* 恒亮 0-100 */
    u8 div;           /* 呼吸分频 ≥1 */
    /* runtime */
    u8 cnt; // 分频计数
    u8 idx; // 分频后计数
} grp_t;

static grp_t g[LED_GROUP_CNT];

/* -- 工具：获取组内 LED 起始线性索引 -- */
static u8 grp_base(u8 grp)
{
    u8 base = 0, i;
    for (i = 0; i < grp; ++i) base += led_group_size[i];
    return base;
}

/* -- 将 4-bit 亮度写入整个组 (软 PWM 0-127) -- */
static void put_lvl(u8 grp, u8 lv4)
{
    u8 base = grp_base(grp);
    u8 cnt  = led_group_size[grp];
    u8 duty = (u8)(lv4 << 3);           /* ×8 */

    while (cnt--) {
        soft_pwm_set_level(base + cnt, duty);
    }
}

/* ========== API 实现 ========== */
void led_group_init(void)
{
    u8 i;
    soft_pwm_init();
    for (i = 0; i < LED_GROUP_CNT; ++i) {
        g[i].mode = LG_MODE_OFF;
        g[i].pct  = 0;
        g[i].div  = 1;
        g[i].idx  = 0;
        g[i].cnt  = 0;
        put_lvl(i, 0);
    }
}

void led_group_set_mode(u8 grp, lg_mode_e m)
{
    if (grp < LED_GROUP_CNT) g[grp].mode = m;
}

void led_group_set_const(u8 grp, u8 pct)
{
    if (grp >= LED_GROUP_CNT) return;
    if (pct > 100) pct = 100;
    g[grp].pct  = pct;
    g[grp].mode = LG_MODE_CONST;
}

void led_group_set_breathe(u8 grp, u8 div)
{
    if (grp >= LED_GROUP_CNT || div == 0) return;
    g[grp].div  = div;
    g[grp].mode = LG_MODE_BREATHE;
}

void led_group_tick_2ms(void)
{
    u8 i;
    for (i = 0; i < LED_GROUP_CNT; ++i) {
        switch (g[i].mode) {

        case LG_MODE_OFF:
            put_lvl(i, 0);
            break;

        case LG_MODE_CONST: {
            u8 duty = (u16)g[i].pct * 127u / 100u;   /* 0-127 */
            put_lvl(i, duty >> 3);                   /* 转 4-bit */
            break;
        }

        case LG_MODE_BREATHE:
            if (++g[i].cnt >= g[i].div) {
                g[i].cnt = 0;
                if (++g[i].idx >= 64) g[i].idx = 0;
            }
            put_lvl(i, lut[g[i].idx]);
            break;
        }
    }
}
