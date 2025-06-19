#ifndef __FSM_H__
#define __FSM_H__

#ifdef __cplusplus
extern "C" {
#endif

void fsm_init(void);
void fsm_tick_1ms(void);
void fsm_loop(void);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
