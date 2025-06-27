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

volatile u16 us_count = 0; // 全局 500us 递增的计数
volatile u16 ms_count = 0; // 全局 1ms 递增的计数

// 主循环用的电池采样标志
volatile u8 batt_due = 0;

static volatile u8 slice1ms = 0;	
static volatile u8 slice2ms = 0;	
static volatile u16 slice100ms = 0;	    

void hal_time_init(void)
{
    //500us@11.0592MHz
	AUXR |= 0x80;			//定时器0 时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x66;				//设置定时初始值
	TH0 = 0xEA;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
    ET0 = 1;             // 使能 Timer-0 中断
}

void timer0_isr(void) interrupt 1 using 1
{	
    // P11 = 1;

    if (++us_count >= 2) {   // 每 1 ms 置位一次
        us_count = 0;

        P11 = !P11;
        
        // 1ms执行一次任务
	    hal_time_tick_1ms();

        led_sm_tick();

        // 2ms执行一次任务
        if(++slice2ms >= 2) {
            slice2ms = 0;
        }
        
        // 500ms执行一次任务
        if (++ms_count >= 500) {   // 每 500 ms 置位一次
            ms_count = 0;
            batt_due = 1; // 电池任务
        }
    }

    // 放到LED状态机模块后面，保证本周期参数正常执行
    soft_pwm_tick();

    // P11 = 0;
}

void hal_time_tick_1ms(void)
{   
    // P11 = 1;

    hal_us_tick_1ms();        /* 推进扫频状态机 */
    soft_timer_tick_1ms();
    touch_service_tick_1ms();
	    

    // P11 = 0;
}
#endif
