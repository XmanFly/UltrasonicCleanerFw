/* 仅编进 Keil 工程；PC 侧不加入 */
#include "platform.h"
#include "common/types.h"
#include <stdarg.h>
#include "hal/uart.h"

char buf[64];        /* temporary print buffer */

void qtPrint(const char *fmt, ...)
{    
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);  /* format into buf (null-terminated) */
    
    /* send the formatted string over UART */
    hal_uart_send_buf((const u8 *)buf);

    va_end(args);

    // (void)fmt;            /* 什么都不做，零代码体积 */
    /* 若要输出串口，可在这里用 vsprintf + putchar 实现 */
}
