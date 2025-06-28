#include "led_group.h"
#include "soft_pwm.h"
#include "common/platform.h"
#include "hal/led.h"

#define TABLE_SIZE 256

/* √4-bit 半正弦 + γ≈sqrt  LUT */
static code const u8 lut[TABLE_SIZE] = {
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,
      2,   3,   3,   3,   3,   3,   4,   4,   4,   5,   5,   5,   6,   6,   6,   7,
      7,   7,   8,   8,   9,   9,  10,  10,  11,  11,  12,  13,  13,  14,  14,  15,
     16,  17,  17,  18,  19,  19,  20,  21,  22,  23,  23,  24,  25,  26,  27,  28,
     29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  41,  42,  43,  44,  45,
     47,  48,  49,  50,  52,  53,  54,  56,  57,  59,  60,  62,  63,  64,  66,  68,
     69,  71,  72,  74,  75,  77,  79,  80,  82,  84,  86,  87,  89,  91,  93,  95,
     97,  98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 121, 123, 125, 127,
    127, 125, 123, 121, 118, 116, 114, 112, 110, 108, 106, 104, 102, 100,  98,  97,
     95,  93,  91,  89,  87,  86,  84,  82,  80,  79,  77,  75,  74,  72,  71,  69,
     68,  66,  64,  63,  62,  60,  59,  57,  56,  54,  53,  52,  50,  49,  48,  47,
     45,  44,  43,  42,  41,  39,  38,  37,  36,  35,  34,  33,  32,  31,  30,  29,
     28,  27,  26,  25,  24,  23,  23,  22,  21,  20,  19,  19,  18,  17,  17,  16,
     15,  14,  14,  13,  13,  12,  11,  11,  10,  10,   9,   9,   8,   8,   7,   7,
      7,   6,   6,   6,   5,   5,   5,   4,   4,   4,   3,   3,   3,   3,   3,   2,
      2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1
};

typedef struct {
    lg_mode_e mode;          /* lg_mode_e */
    u8 pct;           /* 恒亮 0-100 */
    u8 div;           /* 呼吸分频 ≥1 */
    /* runtime */
    u8 cnt; // 分频计数
    u16 idx; // 分频后计数
} grp_t;

static volatile grp_t g[LED_GROUP_CNT];

/* -- 将亮度写入整个组 (软 PWM 0-127) -- */
static void put_lvl(u8 grp, u8 lv)
{
    u8 duty = lv;           /* ×8 */
    soft_pwm_set_level(grp, duty);
    
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

void led_group_tick(void)
{
    volatile u8 i;
    volatile u8 duty;

    for (i = 0; i < LED_GROUP_CNT; ++i) {
        switch (g[i].mode) {
        case LG_MODE_OFF:
            // print("LG_MODE_OFF begin\n");
            put_lvl(i, 0);
            // print("LG_MODE_OFF end\n");
            break;
        case LG_MODE_CONST: {            
            duty = g[i].pct * 128u / 100u; 
            put_lvl(i, duty);
            break;
        }
        case LG_MODE_BREATHE:
            if (++g[i].cnt >= g[i].div) {
                g[i].cnt = 0;
                if (++g[i].idx >= TABLE_SIZE)  {
                    g[i].idx = 0;
                }
            }
            
            // print("LG_MODE_BREATHE id %bu lut %bu idx %bu\n", i, lut[g[i].idx], g[i].idx);
            put_lvl(i, lut[g[i].idx]);
            // print("LG_MODE_BREATHE end\n");
            break;
        }
    }
}
