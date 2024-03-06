#include "printf.h"
#include "peripherals/timer.h"
#include "peripherals/irq.h"
#include "peripherals/aux.h"

const u32 interval_1 = CLOCKHZ;
u32 cur_val_1 = 0;

const u32 interval_3 = CLOCKHZ / 4;
u32 cur_val_3 = 0;

void timer_init(){
    cur_val_1 = REGS_TIMER->counter_lo;
    cur_val_1 += interval_1;
    REGS_TIMER->compare[1] = cur_val_1; // set compare register

    cur_val_3 = REGS_TIMER->counter_lo;
    cur_val_3 += interval_3;
    REGS_TIMER->compare[3] = cur_val_3; // set compare register
}

void handle_timer_1(){
    cur_val_1 += interval_1;
    REGS_TIMER->compare[1] = cur_val_1; 
    REGS_TIMER->control_status |= SYS_TIMER_IRQ_1;

    // printf("Timer 1 received. 1s has passed.\n");
}

void handle_timer_3(){
    cur_val_3 += interval_3;
    REGS_TIMER->compare[3] = cur_val_3; 
    REGS_TIMER->control_status |= SYS_TIMER_IRQ_3;

    // printf("Timer 3 received. 1/4s has passed.\n");
}

u64 timer_get_ticks(){
    u32 hi = REGS_TIMER-> counter_hi;
    u32 lo = REGS_TIMER-> counter_lo;

    // seems better to double check if hi value didn't change after setting
    if(hi != REGS_TIMER->counter_hi){
        hi = REGS_TIMER->counter_hi;
        lo = REGS_TIMER->counter_lo;
    }

    return ((u64)hi << 32) | lo;
}

// sleep in milli seconds.
void timer_sleep(u32 ms){
    u64 s = timer_get_ticks();

    while(timer_get_ticks() < s + (ms*1000));
}