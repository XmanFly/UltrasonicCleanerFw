#include "hal/battery.h"
#include "hal/adc.h"
#include "common/platform.h"

#define ADC_BGV_CH        15u     /* 内部 1.19 V */
#define ADC_BATT_CH       14u     /* P3.6 */
#define ADC_MAX_CNT     1023u     /* 10-bit */
#define BGV_mV          1190u     /* 典型 1.19 V */

// 电压检测去抖
#define FILTER_DELTA      50u       /* 50mV 阈值 */
#define CONFIRM_COUNT      3u       /* 连续超阈值次数，3×100ms = 300ms 确认跳变 */
static volatile u8  confirm_cnt   = 0u;  /* 跳变确认计数 */

static volatile u16 batt_mV = 0u;          /* 当前稳定电压 */

/* --------------- 私有函数 --------------- */
static u16 adc_avg(u8 ch)
{
    volatile u8  i        = 0u;
    volatile u16 sum      = 0u;

    ADC_SetChannel(ch);
    for(i = 0u; i < BAT_SAMPLE_NUM; i++)
    {
        sum += ADC_Read();
    }
    return (u16)(sum / BAT_SAMPLE_NUM);
}
/* --------------------------------------- */

void hal_battery_init(void)
{
    ADC_Init();                    /* ADCCFG 默认 0x20 → Fadc = Fosc/6 ≈ 1.843 MHz */
    P3M0 &= ~(1<<6);               /* P3.6 高阻输入 */
    P3M1 |= (1<<6);
}

/* ---- 读取一次电池电压（mV） ---- */
static u16 read_batt_mv(void)
{
    volatile u16 bg_raw     = 0u;
    volatile u16 bat_raw    = 0u;
    volatile u32 vcc_mv     = 0u;
    volatile u32 vinRaw_mv     = 0u;
    volatile u32 vin_mv     = 0u;

    /* ---------- 采样 ---------- */
    bg_raw  = adc_avg(ADC_BGV_CH);
    if(bg_raw == 0u)
    {
        print("hal bg_raw 0!\n");
        return 0;                    /* 避免除零 */
    }
    bat_raw = adc_avg(ADC_BATT_CH);

    /* ---------- 计算 ---------- */
    vcc_mv  = (u32)BGV_mV * ADC_MAX_CNT / bg_raw;
    vinRaw_mv  = (u32)bat_raw * vcc_mv / ADC_MAX_CNT;
    vin_mv  = vinRaw_mv * (u32)BAT_VOL_RATIO;

    return vin_mv;
}


void hal_battery_task(void)
{
    u16 new_mv, diff;

    new_mv = read_batt_mv();


    /* 计算与上次稳定值的绝对差 */
    if (new_mv > batt_mV)
        diff = new_mv - batt_mV;
    else
        diff = batt_mV - new_mv;

    if (diff <= FILTER_DELTA)
    {
        /* 小于阈值，认为是真正的小幅变化，立即更新 */
        batt_mV = new_mv;
        confirm_cnt = 0u;
    }
    else
    {
        /* 大于阈值，先累加确认计数，连续 N 次才更新 */
        confirm_cnt++;
        if (confirm_cnt >= CONFIRM_COUNT)
        {
            batt_mV = new_mv;
            confirm_cnt = 0u;
        }
        print("hal_battery_task new_mv %u cnt %bu", new_mv, confirm_cnt);
    }

    // print("hal_battery_task bg_raw %u bat_raw %u vcc_mv %lu vinRaw_mv %lu vin_mv %lu\n",
    //         bg_raw, bat_raw, vcc_mv, vinRaw_mv, vin_mv);
}

u16 hal_battery_get_mv(void)
{
    return batt_mV;
}

u8 hal_battery_get_percent(void)
{
    if(batt_mV < 3300u) return 0u;
    if(batt_mV > 4200u) return 100u;
    return (u8)((batt_mV - 3300u) / 9u);  /* 900 mV /100 ≈ 9 mV/格 */
}

u8 hal_battery_is_chg(void)
{
	return 0;
}