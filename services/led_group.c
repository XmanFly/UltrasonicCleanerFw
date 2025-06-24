#include "led_group.h"
#include "soft_pwm.h"
#include "common/platform.h"
#include "hal/led.h"

#define TABLE_SIZE 64

/* √4-bit 半正弦 + γ≈sqrt  LUT */
static code const u8 lut[TABLE_SIZE] = {
      1,1,1,1,1,2,2,3,3,4,4,5,5,6,6,7,
      7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,
     15,15,15,15,15,14,14,13,13,12,12,11,11,10,10,9,
      9,8,8,7,7,6,6,5,5,4,4,3,3,2,2,1
};

/* ------- 默认 IO & 数量表（可改硬件后一起修改） ------- */
const u8 led_group_size[LED_GROUP_CNT] = {LED_RED_GROUP_CNT, LED_BLUE_GROUP_CNT};
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
    volatile base = 0, i;
    for (i = 0; i < grp; ++i) base += led_group_size[i];
    return base;
}

/* -- 将 4-bit 亮度写入整个组 (软 PWM 0-127) -- */
static void put_lvl(u8 grp, u8 lv4)
{
    volatile u8 grp_backup = grp;
    volatile u8 lv4_backup = lv4; // 此处取出比较安全，默认放在寄存器上，后续函数调用会覆盖

    u8 duty = (u8)(lv4_backup << 3);           /* ×8 */
    u8 base = grp_base(grp_backup);
    u8 cnt  = led_group_size[grp_backup];

    while (cnt--) {
        soft_pwm_set_level(base + cnt, duty);
    }
    
    // print("put_lvl id %bu duty %bu base %bu cnt %bu\n", grp_backup, lv4_backup, base, cnt);
}

/* ========== API 实现 ========== */
void led_group_init(void)
{
    u8 i;
    led_init();
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
    volatile u8 i;

    for (i = 0; i < LED_GROUP_CNT; ++i) {
        switch (g[i].mode) {
        case LG_MODE_OFF:
            // print("LG_MODE_OFF begin\n");
            put_lvl(i, 0);
            // print("LG_MODE_OFF end\n");
            break;
        case LG_MODE_CONST: {            
            u8 duty = (u16)g[i].pct * 127u / 100u;   /* 0-127 */
            put_lvl(i, duty >> 3);                   /* 转 4-bit */
            break;
        }
        case LG_MODE_BREATHE:
            if (++g[i].cnt >= g[i].div) {
                g[i].cnt = 0;
                if (++g[i].idx >= TABLE_SIZE) g[i].idx = 0;
            }
            
            // print("LG_MODE_BREATHE id %bu lut %bu idx %bu\n", i, lut[g[i].idx], g[i].idx);
            put_lvl(i, lut[g[i].idx]);
            // print("LG_MODE_BREATHE end\n");
            break;
        }
    }
}
