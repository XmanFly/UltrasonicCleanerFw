#include "logic/fsm.h"
#include "services/soft_timer.h"
#include "services/anim.h"
#include "hal/led.h"
#include "hal/ultrasonic.h"
#include "hal/battery.h"
#include "hal/touch.h"
#include "common/platform.h"

#define LOW_MV  2900
#define WARN_MV 2800
#define FULL_MV 4200
#define CLEAN_MS 180000u

typedef enum{
    OFF,
    WORK,
    CHARGE,
    LOW,
    FINISH,
    ABN
} st_t;
static st_t st;
static int t_clean=-1, t_tmp=-1;

static void enter(st_t s);

static void clean_done(void)
{
    enter(FINISH);
}

static void to_off(void)
{
    enter(OFF);
}

static void enter(st_t s)
{
    qtPrint("fsm enter %d", s);
    if(t_clean>=0) {
        timer_stop(t_clean);
        t_clean=-1;
    }
    if(t_tmp>=0) {
        timer_stop(t_tmp);
        t_tmp=-1;
    }
    st=s;
    switch(s){
    case OFF:
        hal_blue_pwm_set(0);
        {
            u8 i;
            for(i=0;i<9;i++) {
                hal_red_pwm_set(i,0);
            }
        }
        hal_us_stop();
        anim_set(ANIM_NONE,0,0);
        break;
    case WORK:
        anim_set(ANIM_BREATH,3000,0);
        hal_us_start();
        t_clean=timer_start(CLEAN_MS,clean_done,0);
        break;
    case CHARGE:
        anim_set(ANIM_BREATH,2000,0);
        hal_us_stop();
        break;
    case LOW:
        anim_set(ANIM_FLASH,500,250);
        hal_us_stop();
        break;
    case FINISH:
        anim_set(ANIM_BREATH,3000,0);
        t_tmp=timer_start(3000,to_off,0);
        hal_us_stop();
        break;
    case ABN:
        anim_set(ANIM_FLASH,500,250);
        t_tmp=timer_start(3000,to_off,0);
        hal_us_stop();
        break;
    }
}

void fsm_init(void)
{   
    qtPrint("xman\n");

    hal_batt_init();
    hal_touch_init();
    anim_init();
    enter(OFF);
}

void fsm_tick_1ms(void)
{
}

void fsm_loop(void)
{
    soft_timer_task();

    switch(st){
    case OFF:
        if(hal_touch_event() && !hal_batt_is_chg() && hal_batt_get_mv()>WARN_MV) {
            enter(WORK);
        }
        break;
    case WORK:
        if(hal_touch_event()) {
            enter(OFF);
        } else if(hal_batt_get_mv()<LOW_MV) {
            enter(LOW);
        }
        break;
    case CHARGE:
        if(hal_batt_get_mv()<3200) {
            anim_set(ANIM_BREATH,1000,0);
        } else {
            anim_set(ANIM_BREATH,2000,0);
        }
        if(hal_batt_get_mv()>=FULL_MV) {
            anim_set(ANIM_FLASH,1000,1000);
            t_tmp=timer_start(3000,to_off,0);
        }
        break;
    case LOW:
        if(hal_batt_is_chg()) {
            enter(CHARGE);
        }
        break;
    default: break;
    }
}
