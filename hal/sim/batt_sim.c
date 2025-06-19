#include "hal/battery.h"

#ifdef PLATFORM_QT
static u16 mv=3700;
static u8 chg=0;

void hal_batt_init(void)
{

}

u16 hal_batt_get_mv(void)
{
    return mv;
}

u8  hal_batt_is_chg(void)
{
    return chg;
}

void hal_batt_sim_set_mv(u16 v)
{
    mv=v;
}

void hal_batt_sim_set_chg(u8 on)
{
    chg=on;
}
#endif
