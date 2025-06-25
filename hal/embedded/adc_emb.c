#include "hal/adc.h"
#include "include/STC8H.h"  /* STC8H1K08 寄存器定义 */

/**
 * ADC_Init
 * - 10 位分辨率（RESFMT=0）
 * - ADC 时钟 Fosc/(CLKDIV+1)，这里 CLKDIV=7 ⇒ Fosc/8
 * - 采样保持 SAMPCYC = 8 × Tadc
 * - 转换周期 TKCYC = 8 × Tadc
 * - 使能 ADC
 */
void ADC_Init(void)
{
    // ADCCFG: [RESFMT=0]<<7 | CLKDIV[2:0]<<4
    //        = 0x70: 10-bit + CLKDIV=7 ⇒ Fosc/8
    ADCCFG = 0;

    // ADCTIM: [SAMPCYC-1]<<4 | (TKCYC-1)
    //         8 周期 ⇒ SAMPCYC-1 = 7，TKCYC-1 = 7
    // ADCTIM = 0x77;

    // 使能 ADC（ADEN=1），清 ADST/ADIF
    ADC_CONTR = ADC_POWER;  // 0x80
}

/**
 * ADC_SetChannel
 * 在启动转换前，设置待测通道（0~15）
 */
void ADC_SetChannel(u8 ch)
{
    // 保留 ADEN=1，清 ADST/ADIF，写入 CHSEL[3:0]
    ADC_CONTR = ADC_POWER | (ch & 0x0F);
}

/**
 * ADC_Read
 * 启动一次转换并等待完成，返回 10 位结果
 */
u16 ADC_Read(void)
{
    // 启动转换 ADST=1
    ADC_CONTR |= ADC_START; // 0x08

    // 等待 ADST 清零 ⇒ 转换完成
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG));
    ADC_CONTR &= ~ADC_FLAG;

    // 高 8 位在 ADC_RES，低 2 位在 ADC_RESL[1:0]
    return ((u16)ADC_RES << 2) | (ADC_RESL & 0x03);
}
