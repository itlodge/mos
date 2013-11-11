#include "type.h"
#include "process.h"

// External functions
// from ASM
extern void
disp_str(const char *str);

extern void
disp_int(int n);

extern void *
memcpy(void *dst, void *src, int size);

extern char *
strcpy(char *dst, const char *src);

extern void
delay(int t);

extern void
restart();

extern void
enable_irq(int irq);

// from C files
extern void
put_irq_handler(int irq, irq_handler handler);

extern void
clock_handler(int irq);

extern int
get_ticks();

// External variables
extern Process proc_list[PROCESS_NUM];
extern int ticks;

void
testA();

void
testB();

Task task_list[PROCESS_NUM] = {{testA, STACK_SIZE_A, "testA"},
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

    for (int i = 0; i < PROCESS_NUM; ++i) {
        strcpy(p->name, task->name);
        p->pid = i;
        p->ldt_sel = selector_ldt;
        memcpy(&p->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(Descriptor));
        p->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
    
        memcpy(&p->ldts[1], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(Descriptor));
        p->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;

        p->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;
        p->regs.eip = (uint32)task->init_eip;    //
        p->regs.esp = (uint32)task_stack;
        p->regs.eflags = 0x1202;   // ?

        task_stack -= task->stack_size;
        ++p;
        ++task;
        selector_ldt += (1 << 3);
    }
    proc_list[0].ticks = proc_list[0].priority = 150;
    proc_list[1].ticks = proc_list[1].priority = 50;
    
    reenter_cnt = 0;
    proc_ready = proc_list;

    // Initialize 8253 PIT
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (uint8)(TIMER_FREQ / TIMER_HZ));
    out_byte(TIMER0, (uint8)((TIMER_FREQ / TIMER_HZ) >> 8));
    
    put_irq_handler(CLOCK_IRQ, clock_handler);
    enable_irq(CLOCK_IRQ);
    
    restart();

    for (;;) {}
}

void
testA()
{
    while (1) {
        disp_str("A ");
        milli_delay(300);
    }
}

void
testB()
{
    while (1) {
        disp_str("B ");
        milli_delay(300);
    }
}
