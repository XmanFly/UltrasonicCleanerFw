#include "hal/time.h"
#include "hal/uart.h"
#include "logic/fsm.h"
#include "services/soft_pwm.h"
#include "hal/led.h"
#include "hal/battery.h"
#include "hal/ultrasonic.h"
#include "STC8H.h"

void main(void){    
#if UART_ENABLE
    hal_uart_init();
#endif
    hal_battery_init(); // 必须放在定时器0初始化前面，初始化延时用的定时器0
    fsm_init();
    hal_time_init();
    hal_us_init();
	
    EA=1;

	while(1) {
        // P11 = 1;

        fsm_loop(); // 主状态机

        // 电池任务
        if (batt_due) {
            batt_due = 0;
            hal_battery_task(); 
        }        

        // P11 = 0;
    }
}
