#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "type.h"
#include "protect.h"

#define STACK_SIZE_A 0x8000
#define STACK_SIZE_B 0x8000
#define STACK_SIZE_TOTAL (STACK_SIZE_A + STACK_SIZE_B)

typedef struct StackFrame {
    uint32 gs;
    uint32 fs;
    uint32 es;
    uint32 ds;
    uint32 edi;
    uint32 esi;
    uint32 ebp;
    uint32 kernel_esp;
    uint32 ebx;
    uint32 edx;
    uint32 ecx;
    uint32 eax;
    uint32 retaddr;
    uint32 eip;
    uint32 cs;
    uint32 eflags;
    uint32 esp;
    uint32 ss;
} StackFrame;

typedef struct Process {
    StackFrame regs;
    uint16 ldt_sel;     // LDT selector
    Descriptor ldts[LDT_SIZE];
    uint32 pid;         // Process id
    const char name[NAME_LEN];
} Process;

typedef struct Task {
    task_func init_eip;
    int stack_size;
    const char name[NAME_LEN];
} Task;

Process *proc_ready;
Process proc_list[PROCESS_NUM];

#endif /* _PROCESS_H_ */
    
