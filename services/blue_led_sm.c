#include "services/anim.h"
#include "blue_led_sm.h"
/* Internal current mode */
static blue_led_mode_t cur_mode = BLUE_LED_OFF;

void blue_led_sm_init(void)
{
    cur_mode = BLUE_LED_OFF;
    anim_set(LED_BLUE, ANIM_NONE, 0);
}

void blue_led_sm_set(blue_led_mode_t mode)
{
    if(mode == cur_mode)           /* 重复请求 → 忽略 */
        return;

    switch(mode)
    {
    case BLUE_LED_OFF:
        anim_set(LED_BLUE, ANIM_NONE, 0);
        break;

    case BLUE_LED_BREATH_NORMAL:
        if(cur_mode == BLUE_LED_BREATH_FAST)
            /* 仅调周期即可，保证相位连续 */
            anim_update_period(LED_BLUE, 1500);
        else
            anim_set(LED_BLUE, ANIM_BREATH, 1500);
        break;

    case BLUE_LED_BREATH_FAST:
        /* 若已是呼吸动画，仅改周期即可避免闪烁 */
        if(cur_mode == BLUE_LED_BREATH_NORMAL ||
           cur_mode == BLUE_LED_BREATH_FAST)
        {
            anim_update_period(LED_BLUE, 100);
        }
        else
        {
            anim_set(LED_BLUE, ANIM_BREATH, 100);
        }
        break;
    }

    cur_mode = mode;
}