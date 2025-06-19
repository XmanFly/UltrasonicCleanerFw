/* 仅编进 Keil 工程；PC 侧不加入 */
#include "platform.h"

void qtPrint(const char *fmt, ...)
{
    (void)fmt;            /* 什么都不做，零代码体积 */
    /* 若要输出串口，可在这里用 vsprintf + putchar 实现 */
}
