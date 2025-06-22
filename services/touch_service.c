#include "services/touch_service.h"
#include "hal/touch.h"
#include "hal/uart.h"

/* ------------ static state ------------ */
static u8  stable   = 0;      /* debounced level: 1 = pressed          */
static u8  deb_cnt  = 0;      /* 1-ms counter for 50 ms debounce       */
static u16 press_ms = 0;      /* stable press duration (ms)            */
static u8  evt_buf  = TOUCH_EVT_NONE;   /* single-slot event buffer   */

void touch_service_init(void)
{
    stable   = 0;
    deb_cnt  = 0;
    press_ms = 0;
    evt_buf  = TOUCH_EVT_NONE;
}

/* called every 1 ms */
void touch_service_tick_1ms(void)
{
    u8 raw = hal_touch_is_pressed();   /* realtime level from HAL */
    /* 50 ms debounce */
    if(raw)
    {
        if(deb_cnt < 50) deb_cnt++;
        if(deb_cnt == 50 && !stable)
        {
            stable  = 1;
            evt_buf = TOUCH_EVT_EDGE;      /* rising edge event */
        }
    }
    else
    {
        if(deb_cnt) deb_cnt--;
        if(deb_cnt == 0 && stable)
        {
            stable   = 0;
            press_ms = 0;                  /* reset duration */
        }
    }

    /* press-time counting when stable pressed */
    if(stable)
    {
        if(press_ms < 60000)   /* cap at 60 s */
            press_ms++;

        /* generate threshold events once */
        if(press_ms == 500) {
            evt_buf = TOUCH_EVT_PRESS_500;
            qtPrint("touch servce TOUCH_EVT_PRESS_500\r\n");
                        hal_uart_send_buf("TOU");
        }
        if(press_ms == 2000) {
            evt_buf = TOUCH_EVT_PRESS_2S;
            qtPrint("touch servce TOUCH_EVT_PRESS_2S\r\n");
        }
    }

    P11 = ~P11;
}

touch_evt_t touch_service_fetch_event(void)
{
    touch_evt_t e = (touch_evt_t)evt_buf;
    evt_buf = TOUCH_EVT_NONE;
    return e;
}

u16 touch_service_press_ms(void)
{
    return stable ? press_ms : 0;
}
