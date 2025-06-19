#include "hal/time.h"
#include "logic/fsm.h"
#include "STC8H.h"

void main(void){
    hal_time_init();
    fsm_init();
    EA=1;
	while(1) {
        fsm_loop();
    }
}
