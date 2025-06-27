#include "logic/fsm.h"
#include "services/soft_timer.h"
#include "services/touch_service.h"
#include "services/led_sm.h"
#include "hal/led.h"
#include "hal/ultrasonic.h"
#include "hal/battery.h"
#include "hal/uart.h"
#include "hal/power.h"

#ifdef __C51__
#include "include/STC8H.h"
#endif

#define LOW_MV   2900 // 工作过程中低于此电压 蓝灯快闪
#define WORK_MV  3000 // 可工作最低电压
#define FULL_MV  4200 // 满电量电压
#define CHARG_LOW_MV  2800 // 充电时 低于此电量 红灯快闪
#define CHARG_MID_MV  3500 // 充电时 高于此电量 红灯正常闪烁
#define CLEAN_MS 30000u // 清洗时间

typedef enum {
    OFF, // 关机
    WORK, // 清洗
    FINISH, // 工作完成
    CHARGE_INIT, // 充电初始化
    CHARGE_LOW, // 低电量充电
    CHARGE_MID, // 中等电量充电
    CHARGE_FULL, // 充满
    LOW,			// 启动时 电量过低 红灯快闪
    ABN // 异常
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
    power_init();

    led_sm_off(LED_CH_RED);
    led_sm_off(LED_CH_BLUE);
}

/* -------- state transition helper -------- */
static void enter(st_t s)
{
#ifdef PLATFORM_QT    
    if(user_cb) {
        user_cb(s);
    }
    print("fsm enter %d\r\n", s);
#else
    print("fsm enter %bu\r\n", s);
#endif

    exit(st);          /* 离开旧状态 */

    st = s;

    switch(s)
    {
    case OFF:
       led_sm_off(LED_CH_RED);
       led_sm_off(LED_CH_BLUE);
       power_off(); // 断电
#if 0
    //    led_sm_const(LED_CH_RED, 2);
//       led_sm_breathe(LED_CH_RED, BREATH_NORMAL);
    //  led_sm_breathe(LED_CH_BLUE, BREATH_NORMAL);
#endif
        break;

    case WORK:
        hal_us_start();
        led_sm_breathe(LED_CH_BLUE, BREATH_NORMAL);
        t_clean = timer_start(CLEAN_MS, clean_done, 0);
        break;

    case CHARGE_INIT:
        break;

    case CHARGE_LOW:
        led_sm_breathe(LED_CH_RED, BREATH_FAST);
        break;

    case CHARGE_MID:
        led_sm_breathe(LED_CH_RED, BREATH_NORMAL);
        break;

    case CHARGE_FULL:
        led_sm_const(LED_CH_RED, 100);
        break;

    case LOW:
        led_sm_breathe(LED_CH_RED, BREATH_FAST);
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
#ifdef PLATFORM_QT
    print("fsm exit %d\r\n", cur);
#else
    print("fsm exit %bu\r\n", cur);
#endif
    switch(cur)
    {
    case OFF:

    case WORK:
        /* stop ultrasonic generator */
        hal_us_stop();

        led_sm_off(LED_CH_BLUE);

        /* cancel cleaning-finished timer if still active */
        if(t_clean >= 0) { timer_stop(t_clean); t_clean = -1; }
        break;

    case CHARGE_INIT:
        break;

    case CHARGE_LOW:
        led_sm_off(LED_CH_RED);
        break;

    case CHARGE_MID:
        led_sm_off(LED_CH_RED);
        break;

    case CHARGE_FULL:
        led_sm_off(LED_CH_RED);
        break;

    case LOW:
        led_sm_off(LED_CH_RED);
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
        break;

    case FINISH:
        /* no special hardware to release; timer already handled */
        if(t_tmp   >= 0) { timer_stop(t_tmp);   t_tmp   = -1; }
        break;

    case ABN:
        /* abnormal state cleanup (if any future resource) */
        break;

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
            if(!hal_battery_is_chg()) {
                power_on(); // 上电
                print("fsm OFF: not in charge\r\n");
                if(hal_battery_get_mv() > WORK_MV) {
                    print("fsm OFF: mv ok\r\n");
                    enter(WORK);
                    break;
                } else {
                    print("fsm OFF: mv low %u\r\n", hal_battery_get_mv());
                    enter(LOW);
                    break;
                }
            } else {
                print("fsm OFF: in charge\r\n");
            }
        }

        if(hal_battery_is_chg()) {
            enter(CHARGE_INIT);
            break;
        }
        break;

    case WORK:
        if(hal_battery_is_chg()) {
            enter(OFF);
            break;
        }
        if(tev == TOUCH_EVT_PRESS_2S)
        {
            enter(FINISH);
            print("fsm_loop work TOUCH_EVT_PRESS_2S\n");
            break;
        }
		{
			u16 mv = hal_battery_get_mv();
			if(mv < LOW_MV) {
                led_sm_breathe(LED_CH_BLUE, BREATH_FAST);
                print("fsm_loop work bat low %u\n", mv);
            } else if(mv > WORK_MV) {
                led_sm_breathe(LED_CH_BLUE, BREATH_NORMAL);
                // print("fsm_loop work bat high %lu\n", mv);
            }
		}
        break;

    case FINISH:
        break;

    case CHARGE_INIT:
        if(hal_battery_is_chg()) {
            if(hal_battery_get_mv() >= FULL_MV)
            {
                // 电量满
                enter(CHARGE_FULL);
                print("fsm_loop work CHARGE_INIT %u\n", hal_battery_get_mv());
                break;
            } else {
                // 根据当前电量 判断进入哪种充电状态
                if(hal_battery_get_mv() < CHARG_MID_MV) {
                    enter(CHARGE_LOW);
                    break;
                } else {
                    enter(CHARGE_MID);
                    break;
                }
            }
        } else {
            enter(OFF);
            break;
        }
        break;

    case CHARGE_LOW:
        if(!hal_battery_is_chg()) {
            enter(OFF);
            break;
        }

        if(hal_battery_get_mv() >= FULL_MV)
        {
            enter(CHARGE_FULL);
            break;
        } else if(hal_battery_get_mv() > CHARG_MID_MV) {
            enter(CHARGE_MID);
            break;
        }
        break;

    case CHARGE_MID:
        if(!hal_battery_is_chg()) {
            enter(OFF);
            break;
        }

        if(hal_battery_get_mv() >= FULL_MV)
        {
            enter(CHARGE_FULL);
            break;
        } else if(hal_battery_get_mv() < CHARG_LOW_MV) {
            enter(CHARGE_LOW);
            break;
        }
        break;

    case CHARGE_FULL:
        if(!hal_battery_is_chg()) {
            enter(OFF);
            break;
        }
        break;

    case LOW:
        if(hal_battery_is_chg()) {
            enter(CHARGE_INIT);
        }
        break;

    default:
        break;
    }
}
