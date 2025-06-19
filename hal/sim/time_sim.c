#include "hal/time.h"
#include "logic/fsm.h"
#include "services/soft_timer.h"
#include "services/anim.h"

#ifdef PLATFORM_QT

void hal_time_init(void)
{

}

void hal_time_tick_1ms(void)
{
    fsm_tick_1ms();
    soft_timer_tick_1ms();

    static u8 slice2ms = 0;
    if(++slice2ms >= 2) {
        slice2ms = 0;
        anim_tick_2ms();
    }
}
#endif
