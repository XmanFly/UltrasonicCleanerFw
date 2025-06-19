#include "hal/ultrasonic.h"

#ifdef PLATFORM_QT
static u8 on;
void hal_us_start(void)
{
    on=1;
}

void hal_us_stop(void)
{
    on=0;
}

u8 sim_us_on(void)
{
    return on;
};
#endif
