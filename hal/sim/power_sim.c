#include "common/platform.h"
#include "hal/power.h"

static power_cb_tb user_cb = 0;

void power_init()
{
}

void power_on ()
{
    if(user_cb) {
        user_cb(1);
    }
    print("power_on\n");
}

void power_off()
{
    if(user_cb) {
        user_cb(0);
    }
    print("power_off\n");
}

void power_set_callback(power_cb_tb cb)
{
    user_cb = cb;
}
