#include "hal/time.h"
#include "common/types.h"
#include "hal/uart.h"
#include "services/touch_service.h"
#include "services/soft_pwm.h"
#include "services/led_sm.h"
#include "services/soft_timer.h"
#include "hal/ultrasonic.h"

#ifndef PLATFORM_QT
#include "STC8H.h"

volatile u16 ms_count = 0; // 全局 1ms 递增的计数

// 主循环用的电池采样标志
volatile u8 batt_due = 0;

static volatile u8 slice1ms = 0;	
static volatile u16 slice100ms = 0;	    

void hal_time_init(void)
{
    //1000us@11.0592MHz
	AUXR |= 0x80;			// 定时器0 时钟1T模式
	TMOD &= 0xF0;			// 设置定时器模式
	TL0 = 0xCD;				// 设置定时初始值
	TH0 = 0xD4;				// 设置定时初始值
	TF0 = 0;				// 清除TF0标志
	TR0 = 1;				// 定时器0开始计时
    ET0 = 1;                // 使能 Timer-0 中断

#ifndef UART_ENABLE
    //100us@11.0592MHz
    AUXR |= 0x40;			// 定时器1 时钟1T模式
	TMOD &= 0x0F;			// 设置定时器模式
	TL1 = 0xAE;				// 设置定时初始值
	TH1 = 0xFB;				// 设置定时初始值
	TF1 = 0;				// 清除TF1标志
	TR1 = 1;				// 定时器1开始计时
	ET1 = 1;				// 使能定时器1中断    
#endif
}

void timer0_isr(void) interrupt 1 using 1
{	
    // P11 = 1;
        
    // 1ms执行一次任务
    hal_time_tick_1ms();    

    // 500ms执行一次任务
    if (++ms_count >= 500) {   // 每 500 ms 置位一次
        ms_count = 0;
        batt_due = 1; // 电池任务
    }

#ifdef UART_ENABLE
    soft_pwm_tick();
#endif

    // P11 = 0;
}

void Timer1_Isr(void) interrupt 3 using 2
{
    // P11 = 1;

    // 100us执行一次任务
    soft_pwm_tick();

    // P11 = 0;
}


void hal_time_tick_1ms(void)
{   
    // P11 = 1;

    hal_us_tick_1ms();        /* 推进扫频状态机 */
    soft_timer_tick_1ms();
    touch_service_tick_1ms();
	led_sm_tick();
	    
    // P11 = 0;
}
#endif
