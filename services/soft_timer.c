#include "services/soft_timer.h"
#include "common/platform.h"

#define MAX_TMR 4

typedef struct
{
    u32          cnt;
    u32          period;
    timer_cb_t   cb;
    u8           rep;
} tmr_t;

/* large arrays stay in XDATA */
static xdata tmr_t tbl[MAX_TMR];

int timer_start(u32 ms, timer_cb_t cb, u8 rep)
{
    int i;

    for(i = 0; i < MAX_TMR; i++) {
        if(tbl[i].cb == 0) {
            tbl[i].cnt    = ms;
            tbl[i].period = ms;
            tbl[i].cb     = cb;
            tbl[i].rep    = rep ? 1 : 0;
            return i;
        }
    }
    return -1;
}

void timer_stop(int id)
{
    if(id >= 0 && id < MAX_TMR) {
        tbl[id].cb = 0;
    }
}

void soft_timer_tick_1ms(void)
{
    int i;

    for(i = 0; i < MAX_TMR; i++) {
        if(tbl[i].cb && tbl[i].cnt) {
            tbl[i].cnt--;
        }
    }
}

void soft_timer_task(void)
{
    int i;
    timer_cb_t  tmpCb; // 此处必须先备份回调函数,放到最后执行,防止回调执行过程中把定时器删除了,
                       // 然后又分配了定时器,结果回调后,把刚刚分配的定时器清空了

    for(i = 0; i < MAX_TMR; i++) {
        if(tbl[i].cb && (tbl[i].cnt == 0)) {
            tmpCb = tbl[i].cb;

            if(tbl[i].rep) {
                tbl[i].cnt = tbl[i].period;
            }
            else
            {
                tbl[i].cb = 0;
            }

            // 执行回调
            tmpCb();
        }
    }
}
