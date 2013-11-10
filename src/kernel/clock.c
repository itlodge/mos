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
    disp_str("#");
    ++ticks;
    
    if (reenter_cnt != 0) {
        disp_str("!");
        return;
    }
    ++proc_ready;
    if (proc_ready >= proc_list + PROCESS_NUM) {
        proc_ready = proc_list;
    }
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
