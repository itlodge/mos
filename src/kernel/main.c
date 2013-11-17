#include "type.h"
#include "process.h"
#include "extern.h"

// External functions
// from ASM
extern void
restart();

// from C files
extern int
get_ticks();

extern void
init_keyboard();

extern void
init_clock();

extern void
milli_delay(int t);

extern void
task_tty();

// External variables
extern Process proc_list[TASK_NUM + PROCESS_NUM];
extern int ticks;

void
testA();

void
testB();

Task task_list[TASK_NUM] = {{task_tty, STACK_SIZE_TTY, "tty"}};
Task user_proc_list[PROCESS_NUM] = {{testA, STACK_SIZE_A, "testA"},
                                    {testB, STACK_SIZE_B, "testB"}};

int
kernel_main()
{
    disp_str("----kernel_main begin------\n");

    ticks = 0;
    
    Process *p = proc_list;
    Task *task = task_list;
    char *task_stack = task_stack + STACK_SIZE_TOTAL;
    uint16 selector_ldt = SELECTOR_LDT_FIRST;
    uint8 privilege;
    uint8 rpl;
    int eflags;
    
    for (int i = 0; i < PROCESS_NUM + TASK_NUM; ++i) {
        if (i < TASK_NUM) {
            task = task_list + i;
            privilege = PRIVILEGE_TASK;
            rpl = RPL_TASK;
            eflags = 0x1202;
        } else {
            task = user_proc_list + (i - TASK_NUM);
            privilege = PRIVILEGE_USER;
            rpl = RPL_USER;
            eflags = 0x202;
        }
        
        strcpy(p->name, task->name);
        p->pid = i;
        p->ldt_sel = selector_ldt;
        memcpy(&p->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(Descriptor));
        p->ldts[0].attr1 = DA_C | privilege << 5;
        
        memcpy(&p->ldts[1], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(Descriptor));
        p->ldts[1].attr1 = DA_DRW | privilege << 5;

        p->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
        p->regs.eip = (uint32)task->init_eip;    //
        p->regs.esp = (uint32)task_stack;
        p->regs.eflags = eflags;   // ?

        p->tty = 0;
        
        task_stack -= task->stack_size;
        ++p;
        ++task;
        selector_ldt += (1 << 3);
    }
    proc_list[0].ticks = proc_list[0].priority = 15;
    proc_list[1].ticks = proc_list[1].priority = 5;
    proc_list[2].ticks = proc_list[2].priority = 5;
    
    proc_list[1].tty = 0;
    proc_list[2].tty = 1;
    
    reenter_cnt = 0;
    ticks = 0;
    proc_ready = proc_list;

    init_clock();
    init_keyboard();
    
    restart();

    for (;;) {}
}

void
testA()
{
    while (1) {
        printf("A %x", get_ticks());
        milli_delay(300);
    }
}

void
testB()
{
    while (1) {
        printf("B: %x", get_ticks());
        milli_delay(300);
    }
}
