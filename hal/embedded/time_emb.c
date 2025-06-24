#include "hal/time.h"
#include "common/types.h"
#include "hal/uart.h"
#include "services/touch_service.h"
#include "services/soft_pwm.h"
#include "services/led_sm.h"
#include "hal/ultrasonic.h"

#ifndef PLATFORM_QT
#include "STC8H.h"

extern void fsm_tick_1ms(void);
extern void soft_timer_tick_1ms(void);

void hal_time_init(void)
{
    //1毫秒@11.0592MHz
	AUXR |= 0x80;			//定时器0 时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0xCD;				//设置定时初始值
	TH0 = 0xD4;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
    ET0 = 1;             // 使能 Timer-0 中断
}

void timer0_isr(void) interrupt 1 using 1
{	
	hal_time_tick_1ms();
}

void hal_time_tick_1ms(void)
{   
	static u8 slice2ms = 0;	
    static u16 slice1000ms = 0;	
	
    soft_timer_tick_1ms();
    touch_service_tick_1ms();
	soft_pwm_tick_1ms();
    hal_us_tick_1ms();        /* 推进扫频状态机 */

    if(++slice2ms >= 2) {
        slice2ms = 0;
		led_sm_tick_2ms();
    }
	
    // if(++slice1000ms >= 1000) {
    //     slice1000ms = 0;
    //     hal_uart_send_buf("hal");
    // }    

}
#endif
