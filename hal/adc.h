/* adc.h ── STC8H1K08 / Keil-C51 简易 ADC 驱动头文件
 *
 * 只暴露 3 个同步阻塞接口，供 batt_emb.c / 其他模块调用。
 * 若要扩展中断方式、连续采样等，可在此基础上继续添函数。
 */
#ifndef _ADC_H_
#define _ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"

/* 初始化：开启 ADC，配置为 10-bit、单端、Fadc = Fosc / 6
 * Fosc = 11.0592 MHz ⇒ Fadc ≈ 1.843 MHz，完全符合手册 300 k–2 MHz 推荐区。
 */
void ADC_Init(void);

/* 只切换通道号 (0-15)，其它控制位保持不变 */
void ADC_SetChannel(u8 ch);

/* 启动一次转换并阻塞等待结束，返回 0-1023 原始值 */
u16  ADC_Read(void);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* _ADC_H_ */
