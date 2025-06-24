/* adc.c ── 与上面 adc.h 配套的最小阻塞实现 */

#include "hal/adc.h"
#include "include/STC8H.h"      /* 芯片寄存器头文件 */


/*------------------------------------------------
  ADC_Init
  ----------------------------------------------*/
void ADC_Init(void)
{
    /* ADCCFG:
     * 7      6 5 4      3 2 1      0
     * [RESFMT]  CLKDIV[2:0]   RSV   RSV
     * 0x20 ⇒ RESFMT=0(10-bit)，CLKDIV=001(÷6)
     */
    ADCCFG = 0x20;

    /* 使能 ADC (ADEN=1)，其它位暂清 0，
       通道号默认 0；真正采样前 ADC_SetChannel 会重写 */
    ADC_CONTR = 0x80;
}

/*------------------------------------------------
  ADC_SetChannel
  ----------------------------------------------*/
void ADC_SetChannel(u8 ch)
{
    /* 前三位保留(ADEN=1, ADIF=0, ADST=0)，
       下 4 位填写通道号 0-15 */
    ADC_CONTR = 0x80 | (ch & 0x0F);
}

/*------------------------------------------------
  ADC_RunOnce
  ----------------------------------------------*/
u16 ADC_RunOnce(void)
{
    ADC_CONTR |= 0x08;          /* 置 ADST 启动转换 */

    /* 等待 ADST 清零 ⇒ 转换完成 */
    while (ADC_CONTR & 0x08);

    /* 结果高 8 位在 ADC_RES，低 2 位在 ADC_RESL[7:6] */
    return ( (u16)ADC_RES << 2 ) | ( ADC_RESL & 0x03 );
}
