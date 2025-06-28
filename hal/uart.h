/* hal/uart.h ------------------------------------------------------ */
#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include "common/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if UART_ENABLE

/* Initialise UART-1 (STC8H TxD1/P3.4, RxD1/P3.5) to given baud */
void hal_uart_init();

/* Blocking send of one byte */
void hal_uart_send_byte(u8 ch);

/* Blocking send of length-bytes buffer */
void hal_uart_send_buf(const u8 *buf);

#endif

#ifdef __cplusplus
}
#endif
#endif /* __HAL_UART_H__ */
