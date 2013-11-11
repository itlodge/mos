#include "process.h"

extern Process *proc_ready;
extern Process proc_list[PROCESS_NUM];

extern void
disp_str(const char *str);

extern int
get_ticks();

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
