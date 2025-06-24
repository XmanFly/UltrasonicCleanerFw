#include "hal/time.h"
#include "hal/uart.h"
#include "logic/fsm.h"
#include "services/soft_pwm.h"
#include "hal/led.h"
#include "STC8H.h"

void main(void){    
    hal_uart_init();
    fsm_init();
    hal_time_init();
	
    EA=1;
	while(1) {
        fsm_loop();
    }
}
