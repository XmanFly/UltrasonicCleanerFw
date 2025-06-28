#include "hal/time.h"
#include "led_sm.h"
#include "services/soft_timer.h"
#include "services/soft_pwm.h"
#include "touch_service.h"

#ifdef PLATFORM_QT

volatile u16 us_count = 0; // 全局 500us 递增的计数
volatile u16 ms_count = 0; // 全局 1ms 递增的计数

static volatile u8 slice2ms = 0;

volatile u8  batt_due;

void hal_time_init(void)
{

}

void hal_time_tick_1ms(void)
{
//    hal_us_tick_1ms();        /* 推进扫频状态机 */
    soft_timer_tick_1ms();
    touch_service_tick_1ms();
    led_sm_tick();
}

void slot_hal_time_tick()
{
    if (++us_count >= 2) {   // 每 1 ms 置位一次
        us_count = 0;

        // 1ms执行一次任务
        hal_time_tick_1ms();


        // 2ms执行一次任务
        if(++slice2ms >= 2) {
            slice2ms = 0;
        }

        // 500ms执行一次任务
        if (++ms_count >= 500) {   // 每 500 ms 置位一次
            ms_count = 0;
            batt_due = 1; // 电池任务
        }
    }

    // 放到LED状态机模块后面，保证本周期参数正常执行
    soft_pwm_tick();
}

#endif
