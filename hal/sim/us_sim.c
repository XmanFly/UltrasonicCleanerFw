#include "hal/ultrasonic.h"

#ifdef PLATFORM_QT
static u8 on;
static hal_us_cb_tb user_cb = 0;
void hal_us_start(void)
{
    on=1;
    if(user_cb) {
        user_cb(on);
    }
}

void hal_us_stop(void)
{
    on=0;
    if(user_cb) {
        user_cb(on);
    }
}

u8 sim_us_on(void)
{
    return on;
}

void hal_us_set_callback(hal_us_cb_tb cb)
{
    user_cb = cb;
}

#endif
