/* hal/embedded/uart_emb.c ---------------------------------------- */
#include "hal/uart.h"
#ifndef PLATFORM_QT
#include "STC8H.h"

#if UART_ENABLE    

static void uart_delay(void) { _nop_(); }   /* tiny wait helper */

/* ----------------------------------------------------------------
 * UART-1 init
 * ----------------------------------------------------------------*/
void hal_uart_init()
{ 
    /* ---------- GPIO ---------- */
    P_SW1 &= 0x3F;      // 选择串口1引脚：P3.0(RxD)和P3.1(TxD)

#if 0 // 定时器1 高精度 19200
    /* 1. 串口0：8-N-1 */
    SCON = 0x50;

    /* 2. 使能 SMOD = 1，倍频 */
    PCON |= 0x80;          // bit7 = 1

    /* 3. Timer-1 -> 模式2 (8位自动重装) */
    TMOD &= ~0xF0;         // 清 T1 位
    TMOD |=  0x20;         // 置模式2

    /* 4. 1-T 时钟源 */
    AUXR |=  0x40;         // T1x12 = 1
    AUXR &= ~0x01;         // UART0 用 T1

    /* 5. 计算重装值：256 - Fosc / (8×baud) */
    TH1 = TL1 = 0xDC;      // 0xFA → 115200 bps

    /* 6. 启动定时器1 */
    TR1 = 1;
    TI  = 1;
#endif

#if 1 // 定时器1 115200
    SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xE8;			//设置定时初始值
	TH1 = 0xFF;			//设置定时初始值
	ET1 = 0;			//禁止定时器中断
	TR1 = 1;			//定时器1开始计时
#endif

#if 0 // 定时器2 19200
    /* -------- SCON: 8-N-1, 允许接收 -------- */
    SCON = 0x50;                    /* SM0=0 SM1=1 REN=1 */

    /* -------- Timer-2: 16-bit auto-reload, 1 T, 内部时钟 -------- */
    AUXR |=  0x04;                  /* T2x12 = 1 → 1 T 模式          */
    AUXR |=  0x01;                  /* UART0 用 T2        */
    T2H   = 0xFF;
    T2L   = 0x70;
    AUXR |=  T2R;                  /* T2R   = 1 → 启动 T2           */
#endif
}

/* ----------------------------------------------------------------
 * Blocking send one byte
 * ----------------------------------------------------------------*/
void hal_uart_send_byte(u8 ch)
{     
    SBUF = ch;
    while(!TI);             /* wait previous byte */    
    TI = 0;    
}

/* ----------------------------------------------------------------
 * Blocking send buffer
 * ----------------------------------------------------------------*/
void hal_uart_send_buf(const u8 *buf)
{
    while(*buf)
    {
        hal_uart_send_byte(*buf++);
    }
}

#endif

#endif  /* PLATFORM_QT */
