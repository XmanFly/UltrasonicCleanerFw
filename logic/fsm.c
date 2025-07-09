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
#define WORK_MV  2800 // 可工作最低电压
#define FULL_MV  4100 // 满电量电压
#define RECHARGE_MV  4000 // 满电量后再充电电压
#define CHARG_LOW_MV  3200 // 充电时 低于此电量 红灯快闪
#define CHARG_MID_MV  3500 // 充电时 高于此电量 红灯正常闪烁
#define CLEAN_MS 180000u // 清洗时间
#define CLEAN_LOW_WARN 3000u // 电压过低状态持续时间
#define CHARGE_CONFIRM_TIME 100u // 插上适配器后电压稳定时间
#define AD_DELAY_TIME 10u
#define CHARGE_CTRL_ON_TIME 150u // 充电电压开时间
#define CHARGE_CTRL_OFF_TIME 1850u // 充电电压关时间


typedef enum {
    INIT, // 初始化 0
    IDLE, // 空闲 1
    AD_DELAY, // AD检测延时 2
    WORK, // 清洗 3
    FINISH, // 工作完成 4
    OFF, // 关机 5
    CHARGE_CONFIRM, // 充电等待电压稳定 6
    CHARGE_INIT, // 充电初始化 7
    CHARGE_CTRL_ON, // 充电电压高(断电) 8
    CHARGE_CTRL_OFF, // 充电电压低(充电) 9
    CHARGE_FULL, // 充满 10
    LOW, // 启动时 电量过低 红灯快闪 11
    ABN // 异常 12
} st_t;

static volatile st_t  st = INIT;
static volatile int   t_clean = -1;
static volatile int   t_tmp   = -1;

/* forward */
static void enter(st_t s);
static void exit(st_t cur);
static void clean_done(void) { enter(FINISH); }
static void ad_delay_done(void) 
{
    if(hal_battery_get_mv() > WORK_MV) {
        print("fsm OFF: mv ok\r\n");
        enter(WORK);
    } else {
        print("fsm OFF: mv low %u\r\n", hal_battery_get_mv());
        enter(LOW);
    }
}
static void charge_confirm_done(void) 
{
    if(hal_battery_is_chg()) {
        enter(CHARGE_INIT);
    } else {
        enter(OFF);
    }
}
static void to_idle(void)     { enter(IDLE);   }
static void to_off(void)     { enter(OFF);   }
static void to_charge_init(void)     { enter(CHARGE_INIT);   }
static void to_charge_ctrl_off(void)     { enter(CHARGE_CTRL_OFF);   }
static void to_charge_ctrl_on(void)     { enter(CHARGE_CTRL_ON);   }

void charge_handle();

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

    enter(INIT);
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
    case INIT:
        power_on(); // 上电
        t_tmp = timer_start(1, to_idle, 0);
        break;

    case IDLE:
        break;

    case AD_DELAY:
        t_tmp = timer_start(AD_DELAY_TIME, ad_delay_done, 0);
        break;

    case WORK:
        hal_us_start();
        led_sm_breathe(LED_CH_BLUE, BREATH_NORMAL);
        t_clean = timer_start(CLEAN_MS, clean_done, 0);
        break;

    case FINISH:
        t_tmp = timer_start(1000, to_off, 0);
        break;

    case OFF:
        led_sm_off(LED_CH_RED);
        led_sm_off(LED_CH_BLUE);
        power_off(); // 断电
        t_tmp = timer_start(10, to_idle, 0);
        break;

    case CHARGE_CONFIRM:
        power_off(); // 关闭供电自锁 适配器插上后必然进入此状态
        t_tmp = timer_start(CHARGE_CONFIRM_TIME, charge_confirm_done, 0);
        break;

    case CHARGE_INIT:
		hal_battery_set_port_output(); // 设置端口为输出
        t_tmp = timer_start(1, to_charge_ctrl_off, 0);
        break;

    case CHARGE_CTRL_ON:
        hal_battery_set_chg(1);
        t_tmp = timer_start(CHARGE_CTRL_ON_TIME, to_charge_ctrl_off, 0);
        break;

    case CHARGE_CTRL_OFF:
        hal_battery_set_chg(0);
        t_tmp = timer_start(CHARGE_CTRL_OFF_TIME, to_charge_ctrl_on, 0);
        break;

    case CHARGE_FULL:
        hal_battery_set_chg(1); // 停止充电
        led_sm_const(LED_CH_RED, 100);
        break;

    case LOW:
        led_sm_breathe(LED_CH_RED, BREATH_FAST);
        t_tmp = timer_start(CLEAN_LOW_WARN, to_off, 0);
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
    case INIT:
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
        break;

    case IDLE:
        break;

    case AD_DELAY:
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
        break;        

    case WORK:
        /* stop ultrasonic generator */
        hal_us_stop();

        led_sm_off(LED_CH_BLUE);

        /* cancel cleaning-finished timer if still active */
        if(t_clean >= 0) { timer_stop(t_clean); t_clean = -1; }
        break;

    case FINISH:
        /* no special hardware to release; timer already handled */
        if(t_tmp   >= 0) { timer_stop(t_tmp);   t_tmp   = -1; }
        break;

    case OFF:
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
        break;

    case CHARGE_CONFIRM:
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
        break;

    case CHARGE_INIT:
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
        break;

    case CHARGE_CTRL_ON:
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
        break;

    case CHARGE_CTRL_OFF:
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
        break;

    case CHARGE_FULL:
        led_sm_off(LED_CH_RED);
        break;

    case LOW:
        led_sm_off(LED_CH_RED);
        if(t_tmp >= 0) { timer_stop(t_tmp); t_tmp = -1; }
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
    case INIT:
        break;

    case IDLE:
        // 插上适配器 进入充电
        if(hal_battery_is_chg()) {
            enter(CHARGE_CONFIRM);
            break;
        }
        // 适配器未连接 收到按键信号
        if(tev == TOUCH_EVT_PRESS_500) {
            enter(AD_DELAY);
            break;
        }
        break;

    case AD_DELAY:
        // 插上适配器 进入充电
        if(hal_battery_is_chg()) {
            enter(CHARGE_CONFIRM);
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
                // print("fsm_loop work bat low %u\n", mv);
            } else if(mv > WORK_MV) {
                led_sm_breathe(LED_CH_BLUE, BREATH_NORMAL);
                // print("fsm_loop work bat high %lu\n", mv);
            }
        }
        break;

    case FINISH:
        break;

    case OFF:
        break;

    case CHARGE_CONFIRM:
        if(!hal_battery_is_chg()) {
            enter(OFF);
            break;
        }
        break;        

    case CHARGE_INIT:
        break;

    case CHARGE_CTRL_ON:
        charge_handle();
        break;

    case CHARGE_CTRL_OFF:
        charge_handle();
        break;

    case CHARGE_FULL:
        if(hal_battery_get_mv() < RECHARGE_MV) {
            enter(CHARGE_INIT);
        }
        break;

    case LOW:
        if(hal_battery_is_chg()) {
            enter(CHARGE_CONFIRM);
        }
        break;

    default:
        break;
    }
}

void charge_handle()
{
    if(hal_battery_get_mv() >= FULL_MV)
    {
        // 电量满
        enter(CHARGE_FULL);
        print("fsm_loop work CHARGE_INIT, enter CHARGE_FULL %u\n", hal_battery_get_mv());
    } else {
        // 根据当前电量 设置灯颜色
        // if(hal_battery_get_mv() < CHARG_MID_MV) {
        //     led_sm_breathe(LED_CH_RED, BREATH_FAST);
        // } else {
        //     led_sm_breathe(LED_CH_RED, BREATH_NORMAL);
        // }
        led_sm_breathe(LED_CH_RED, BREATH_NORMAL);
    }
}
