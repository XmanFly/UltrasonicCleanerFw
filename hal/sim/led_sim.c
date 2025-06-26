#include "hal/led.h"
#include "services/led_group.h"

#ifdef PLATFORM_QT

static led_cb_tb user_cb = 0;

const volatile LedIo_t led_io_map[LED_TOTAL] = {
    {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6},
    {1, 0}
};
void led_init()
{

}

void led_on (u8 id)
{
    if(user_cb) {
        user_cb(id, 1);
    }
}

void led_off(u8 id)
{
    if(user_cb) {
        user_cb(id, 0);
    }
}

void led_set_callback(led_cb_tb cb)
{
    user_cb = cb;
}

#endif
