#include "hal/battery.h"
#include "hal/adc.h"

#define ADC_BGV_CH        15u     /* 内部 1.19 V */
#define ADC_BATT_CH       14u     /* P3.6 */
#define ADC_MAX_CNT     1023u     /* 10-bit */
#define BGV_mV          1190u     /* 典型 1.19 V */

static u16 batt_mV = 0u;          /* 最近一次结果 */

/* --------------- 私有函数 --------------- */
static u16 adc_avg(u8 ch)
{
    u8  i        = 0u;
    u16 sum      = 0u;

    ADC_SetChannel(ch);
    for(i = 0u; i < BAT_SAMPLE_NUM; i++)
    {
        sum += ADC_RunOnce();
    }
    return (u16)(sum / BAT_SAMPLE_NUM);
}
/* --------------------------------------- */

void hal_battery_init(void)
{
    ADC_Init();                    /* ADCCFG 默认 0x20 → Fadc = Fosc/6 ≈ 1.843 MHz */
    P3M0 &= ~(1<<6);               /* P3.6 高阻输入 */
    P3M1 &= ~(1<<6);
}

void hal_battery_task(void)
{
    u16 bg_raw     = 0u;
    u16 bat_raw    = 0u;
    u32 vcc_mv     = 0u;
    u32 vin_mv     = 0u;

    /* ---------- 采样 ---------- */
    bg_raw  = adc_avg(ADC_BGV_CH);
    if(bg_raw == 0u)
    {
        return;                    /* 避免除零 */
    }
    bat_raw = adc_avg(ADC_BATT_CH);

    /* ---------- 计算 ---------- */
    vcc_mv  = (u32)BGV_mV * ADC_MAX_CNT / bg_raw;
    vin_mv  = (u32)bat_raw * vcc_mv / ADC_MAX_CNT;
    vin_mv  = vin_mv * (BAT_R1_KOHM + BAT_R2_KOHM) / BAT_R2_KOHM;

    batt_mV = (u16)vin_mv;
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

u8 hal_batt_is_chg(void)
{
	return 0;
}