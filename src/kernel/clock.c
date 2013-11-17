#include "process.h"
#include "extern.h"

extern Process *proc_ready;
extern Process proc_list[PROCESS_NUM + TASK_NUM];

extern int
get_ticks();

extern void
put_irq_handler(int irq, irq_handler handler);

extern int ticks;

void
clock_handler(int irq)
{
    ++ticks;
    --proc_ready->ticks;
    
    if (reenter_cnt != 0) {
        return;
    }
    if (proc_ready->ticks > 0) {
        return;
    }
    schedule();
}

void
milli_delay(int ms)
{
    int t = get_ticks();
    
    while (true) {
        int diff = get_ticks() - t;
        if (diff * 1000 / TIMER_HZ > ms) {
            break;
        }
    }
}

void
init_clock()
{
    // Initialize 8253 PIT
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (uint8)(TIMER_FREQ / TIMER_HZ));
    out_byte(TIMER0, (uint8)((TIMER_FREQ / TIMER_HZ) >> 8));
    
    put_irq_handler(CLOCK_IRQ, clock_handler);
    enable_irq(CLOCK_IRQ);
}
