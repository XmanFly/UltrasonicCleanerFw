#include "logic/fsm.h"
#include "services/soft_timer.h"
#include "services/anim.h"
#include "services/touch_service.h"
#include "hal/led.h"
#include "hal/ultrasonic.h"
#include "hal/battery.h"

#define LOW_MV   2900 // 工作过程中低于此电压 蓝灯快闪
#define WARN_MV  2800
#define FULL_MV  4200
#define CLEAN_MS 10000u // 清洗时间

typedef enum {
    OFF,
    WORK,
    CHARGE,
    LOW,
    FINISH,
    ABN
} st_t;

static st_t  st;
static int   t_clean = -1;
static int   t_tmp   = -1;

/* forward */
static void enter(st_t s);
static void exit(st_t cur);
static void clean_done(void) { enter(FINISH); }
static void to_off(void)     { enter(OFF);   }


#ifdef PLATFORM_QT
static fsm_cb_tb user_cb = 0;
void fsm_set_callback(fsm_cb_tb cb)
{
    user_cb = cb;
}
#endif

/* -------- state transition helper -------- */
static void enter(st_t s)
{
#ifdef PLATFORM_QT
    qtPrint("fsm enter %d\r\n", s);
    if(user_cb) {
        user_cb(s);
    }
#endif

    exit(st);          /* 离开旧状态 */

    st = s;

    switch(s)
    {
    case OFF:
        break;

    case WORK:
        hal_us_start();
        t_clean = timer_start(CLEAN_MS, clean_done, 0);
        anim_set(LED_BLUE, ANIM_BREATH, 1500);     /* 1.5 s 呼吸 */
        break;

    case CHARGE:
        anim_update_period(LED_RED, 100);
        break;

    case LOW:
        anim_set(LED_RED,  ANIM_NONE, 0);
        anim_set(LED_BLUE, ANIM_NONE, 0);
        break;

    case FINISH:
//        anim_set(ANIM_BREATH, 3000, 0);
        t_tmp = timer_start(1000, to_off, 0);
        break;

    case ABN:
        t_tmp = timer_start(3000, to_off, 0);
        break;
    }
}

/* -------------------------------------------------------------
 * Gracefully leave the current state: stop peripherals, reset
 * LEDs, cancel timers that belong to *cur* only.
 * ----------------------------------------------------------- */
static void exit(st_t cur)
{
    qtPrint("fsm exit %d\r\n", cur);
    switch(cur)
    {
    case WORK:
        /* stop ultrasonic generator */
        hal_us_stop();

        anim_set(LED_BLUE, ANIM_NONE, 0);     /* 关闭蓝灯 */

        /* cancel cleaning-finished timer if still active */
        if(t_clean >= 0) { timer_stop(t_clean); t_clean = -1; }
        break;

    case CHARGE:
        /* charging-done timer */
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
        break;

    case LOW:
        /* ensure fast-flash animation stops */
        anim_set(ANIM_NONE, 0, 0);
        break;

    case FINISH:
        /* no special hardware to release; timer already handled */
        if(t_tmp   >= 0) { timer_stop(t_tmp);   t_tmp   = -1; }
        break;

    case ABN:
        /* abnormal state cleanup (if any future resource) */
        break;

    case OFF:
    default:
        /* OFF and unknown states need no extra cleanup */
        break;
    }
}


void fsm_init(void)
{
    hal_batt_init();
    touch_service_init();
    led_hw_init();
    anim_init();
    led_hw_start(); /* ★ 新：在第一次用灯效前一定初始化 */
    enter(OFF);
}

void fsm_loop(void)
{
	touch_evt_t tev;
	
    soft_timer_task();

    /* fetch at most one touch event per loop */
    tev = touch_service_fetch_event();

    switch(st)
    {
    case OFF:
        if(tev == TOUCH_EVT_PRESS_500 &&
           !hal_batt_is_chg() &&
           hal_batt_get_mv() > WARN_MV)
        {
            enter(WORK);
        }

        if(hal_batt_is_chg()) {
            enter(CHARGE);
        }
        break;

    case WORK:
        if(tev == TOUCH_EVT_PRESS_2S)
        {
            enter(FINISH);
        }
        if(hal_batt_get_mv() < LOW_MV)
        {
            anim_set(LED_BLUE, ANIM_BREATH, 100);     /* 蓝灯快闪 */
        }

        break;

    case CHARGE:
        if(hal_batt_get_mv() < 3200)
            anim_set(ANIM_BREATH, 1000, 0);
        else
            anim_set(ANIM_BREATH, 2000, 0);

        if(hal_batt_get_mv() >= FULL_MV)
        {
            // 设置灯亮度
            t_tmp = timer_start(3000, to_off, 0);
        }

        if(!hal_batt_is_chg()) {
            enter(OFF);
        }
        break;

    case LOW:
        if(hal_batt_is_chg())
            enter(CHARGE);
        break;

    default:
        break;
    }
}
