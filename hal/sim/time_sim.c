#include "hal/time.h"

#ifdef PLATFORM_QT
extern void soft_timer_tick_1ms(void);
void hal_time_init(void)
{

}
void hal_time_tick_1ms(void)
{
    soft_timer_tick_1ms();
}
#endif
