#include "hal/touch.h"

#ifdef PLATFORM_QT
static u8 flag;
void hal_touch_init(void)
{
    flag=0;
}

u8 hal_touch_event(void)
{
    u8 f=flag;
    flag=0;
    return f;
}

void hal_touch_sim_trigger(void)
{
    flag=1;
}
#endif
