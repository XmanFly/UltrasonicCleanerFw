#include "logic/fsm.h"
#include "services/soft_timer.h"
#include "services/touch_service.h"
#include "services/led_sm.h"
#include "hal/led.h"
#include "hal/ultrasonic.h"
#include "hal/battery.h"
#include "hal/uart.h"

#ifdef __C51__
#include "include/STC8H.h"
#endif

#define LOW_MV   2900 // 工作过程中低于此电压 蓝灯快闪
#define LOW_HYST_MV  3000    /* 退出低电警告 (LOW_MV + 100 mV) */
#define FULL_MV  4200
#define CHARG_LOW_MV  2800 // 充电时 低于此电量 红灯快闪
#define CHARG_MID_MV  3500 // 充电时 高于此电量 红灯正常闪烁
#define CLEAN_MS 3000u // 清洗时间

typedef enum {
    OFF,
    WORK,
    FINISH,
    CHARGE,
    CHARGE_FULL,
    LOW,			// 启动时 电量过低 红灯快闪
    ABN
} st_t;

static st_t  st = OFF;
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


void fsm_init(void)
{
    led_sm_init();
    touch_service_init();
    timer_int();
    enter(OFF);
}

/* -------- state transition helper -------- */
static void enter(st_t s)
{
#ifdef PLATFORM_QT    
    if(user_cb) {
        user_cb(s);
    }
#endif
    print("fsm enter %bu\r\n", s);

    exit(st);          /* 离开旧状态 */

    st = s;

    switch(s)
    {
    case OFF:
    //    led_sm_off(LED_CH_RED);
    //    led_sm_const(LED_CH_RED, 50);
        led_sm_breathe(LED_CH_RED, 1);
        break;

    case WORK:
        hal_us_start();
        // led_sm_const(LED_CH_BLUE, 100);
        t_clean = timer_start(CLEAN_MS, clean_done, 0);
        break;

    case CHARGE:
        break;

    case CHARGE_FULL:
        // led_sm_breathe(LED_CH_BLUE, 1);
        break;

    case LOW:
        // led_sm_breathe(LED_CH_RED, 1);
        t_tmp = timer_start(6000, to_off, 0);
        break;

    case FINISH:
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
    print("fsm exit %bu\r\n", cur);
    switch(cur)
    {
    case WORK:
        /* stop ultrasonic generator */
        hal_us_stop();

        // led_sm_breathe(LED_CH_BLUE, 1);

        /* cancel cleaning-finished timer if still active */
        if(t_clean >= 0) { timer_stop(t_clean); t_clean = -1; }
        break;

    case CHARGE:
        // led_sm_breathe(LED_CH_BLUE, 1);

        /* charging-done timer */
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
        break;

    case CHARGE_FULL:
        // led_sm_breathe(LED_CH_BLUE, 1);
        break;

    case LOW:
        // led_sm_breathe(LED_CH_BLUE, 1);
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
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

void fsm_loop(void)
{
	touch_evt_t tev;

#ifdef __C51__
//    P11 = !P11;
#endif

    soft_timer_task();

    /* fetch at most one touch event per loop */
    tev = touch_service_fetch_event();

    switch(st)
    {
    case OFF:
        if(tev == TOUCH_EVT_PRESS_500) {
            print("fsm OFF: TOUCH_EVT_PRESS_500\r\n");
            if(!hal_batt_is_chg()) {
                print("fsm OFF: not in charge\r\n");
                if(hal_battery_get_mv() > LOW_MV) {
                    print("fsm OFF: mv ok\r\n");
                    enter(WORK);
                } else {
                    print("fsm OFF: mv low\r\n");
                    enter(LOW);
                }
            } else {
                print("fsm OFF: in charge\r\n");
            }
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
		{
			u16 mv = hal_battery_get_mv();
			// if(mv < LOW_MV)
			// 	// led_sm_breathe(LED_CH_BLUE, 1);
			// else if(mv > LOW_HYST_MV)
			// 	// led_sm_breathe(LED_CH_BLUE, 1);
		}
        break;

    case FINISH:
        break;

    case CHARGE:
        if(hal_battery_get_mv() < CHARG_LOW_MV) {
            led_sm_breathe(LED_CH_RED, 1);
        } else if(hal_battery_get_mv() > CHARG_MID_MV) {
            led_sm_breathe(LED_CH_RED, 1);
        }

        if(hal_battery_get_mv() >= FULL_MV)
        {
            enter(CHARGE_FULL);
        }

        if(!hal_batt_is_chg()) {
            enter(OFF);
        }
        break;

    case CHARGE_FULL:
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
