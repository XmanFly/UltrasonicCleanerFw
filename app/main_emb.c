#include "hal/time.h"
#include "hal/uart.h"
#include "logic/fsm.h"
#include "services/soft_pwm.h"
#include "hal/led.h"
#include "hal/battery.h"
#include "hal/ultrasonic.h"
#include "STC8H.h"

void main(void){    
    hal_uart_init();
    fsm_init();
    hal_time_init();
    // hal_battery_init();
    hal_us_init();

    // hal_us_start();
	
    EA=1;

	while(1) {
        fsm_loop();     // 主状态机
        // hal_battery_task(); // 电池任务
    }
}
