#include "process.h"

extern void
disp_str(const char *str);

extern int ticks;
extern Process proc_list[PROCESS_NUM];
extern Process *proc_ready;

int
sys_get_ticks()
{
    return ticks;
}

void
schedule()
{
    int max_ticks = 0;

    while (max_ticks <= 0) {
        for (Process *p = proc_list; p < proc_list + PROCESS_NUM; ++p) {
            if (p->ticks > max_ticks) {
                max_ticks = p->ticks;
                proc_ready = p;
            }
        }
        if (max_ticks <= 0) {
            for (Process *p = proc_list; p < proc_list + PROCESS_NUM; ++p) {
                p->ticks = p->priority;
            }
        }
    }
}
