#include "hal/led.h"
#include "services/led_group.h"

#ifdef PLATFORM_QT

static led_cb_tb user_cb = 0;

void led_init()
{

}

void led_on (u8 grp)
{
    if(user_cb) {
        user_cb(grp, 1);
    }
}

void led_off(u8 grp)
{
    if(user_cb) {
        user_cb(grp, 0);
    }
}

void led_set_callback(led_cb_tb cb)
{
    user_cb = cb;
}

#endif
