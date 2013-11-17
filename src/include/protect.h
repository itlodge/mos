#ifndef _PROTECT_H_
#define _PROTECT_H_

#include "type.h"
#include "const.h"

// Descriptor
typedef struct Descriptor {
    uint16 limit_low;
    uint16 base_low;
    uint8 base_mid;
    uint8 attr1;
    uint8 limit_high_attr2;
    uint8 base_high;
} Descriptor;

// Gate
typedef struct Gate {
    uint16 offset_low;
    uint16 selector;
    uint8 dw_cnt;
    uint8 attr;
    uint16 offset_high;
} Gate;

typedef struct Tss {
    uint32 backlink;
    uint32 esp0;   /* stack pointer to use during interrupt */
    uint32 ss0;    /*   "   segment  "  "    "        "     */
    uint32 esp1;
    uint32 ss1;
    uint32 esp2;
    uint32 ss2;
    uint32 cr3;
    uint32 eip;
    uint32 flags;
    uint32 eax;
    uint32 ecx;
    uint32 edx;
    uint32 ebx;
    uint32 esp;
    uint32 ebp;
    uint32 esi;
    uint32 edi;
    uint32 es;
    uint32 cs;
    uint32 ss;
    uint32 ds;
    uint32 fs;
    uint32 gs;
    uint32 ldt;
    uint16 trap;
    uint16 iobase; 
} Tss;
    
// Descriptor type
#define DA_LDT                  0x82    // LDT
#define DA_TaskGate             0x85    // Task gate
#define DA_386TSS               0x89    // Task state segment
#define DA_386CGate             0x8C    // Call gate
#define DA_386IGate             0x8E    // 386 Interrupt

// Descriptor attributes
#define DA_DR                   0x90    /* 存在的只读数据段类型值               */
#define DA_DRW                  0x92    /* 存在的可读写数据段属性值             */
#define DA_DRWA                 0x93    /* 存在的已访问可读写数据段类型值       */
#define DA_C                    0x98    /* 存在的只执行代码段属性值             */
#define DA_CR                   0x9A    /* 存在的可执行可读代码段属性值 */
#define DA_CCO                  0x9C    /* 存在的只执行一致代码段属性值 */
#define DA_CCOR                 0x9E    /* 存在的可执行可读一致代码段属性值     */

// Descriptor index
#define INDEX_DUMMY             0       
#define INDEX_FLAT_C            1       
#define INDEX_FLAT_RW           2       
#define INDEX_VIDEO             3       
#define INDEX_TSS               4
#define INDEX_LDT_FIRST         5

// DT
#define DT_PTR_LEN 6
#define GDT_SIZE 128
#define IDT_SIZE 256
#define LDT_SIZE 2

// Selectors
#define SELECTOR_DUMMY             0            
#define SELECTOR_FLAT_C         0x08            
#define SELECTOR_FLAT_RW        0x10            
#define SELECTOR_VIDEO          (0x18+3)        
#define SELECTOR_LDT_FIRST      0x28

#define SELECTOR_KERNEL_CS      SELECTOR_FLAT_C
#define SELECTOR_KERNEL_DS      SELECTOR_FLAT_RW
#define SELECTOR_KERNEL_GS      SELECTOR_VIDEO

// Selector Atrributes
#define SA_RPL_MASK 0xFFFC
#define SA_RPL0         0
#define SA_RPL1         1
#define SA_RPL2         2
#define SA_RPL3         3

#define SA_TI_MASK      0xFFFB
#define SA_TIG          0
#define SA_TIL          4

// Interrupt

// Port
#define INT_M_CTL 0x20 // I/O port for interrupt controller(Master)
#define INT_M_CTLMASK 0x21 //
#define INT_S_CTL 0xA0 // I/O port for thee second interrupt controller(Slave)
#define INT_S_CTLMASK 0xA1

// Privilege
#define PRIVILEGE_KRNL  0
#define PRIVILEGE_TASK  1
#define PRIVILEGE_USER  3

// RPL
#define RPL_KRNL SA_RPL0
#define RPL_TASK SA_RPL1
#define RPL_USER SA_RPL3

// Vectors
#define INT_VECTOR_DIVIDE               0x0
#define INT_VECTOR_DEBUG                0x1
#define INT_VECTOR_NMI                  0x2
#define INT_VECTOR_BREAKPOINT           0x3
#define INT_VECTOR_OVERFLOW             0x4
#define INT_VECTOR_BOUNDS               0x5
#define INT_VECTOR_INVAL_OP             0x6
#define INT_VECTOR_COPROC_NOT           0x7
#define INT_VECTOR_DOUBLE_FAULT         0x8
#define INT_VECTOR_COPROC_SEG           0x9
#define INT_VECTOR_INVAL_TSS            0xA
#define INT_VECTOR_SEG_NOT              0xB
#define INT_VECTOR_STACK_FAULT          0xC
#define INT_VECTOR_PROTECTION           0xD
#define INT_VECTOR_PAGE_FAULT           0xE
#define INT_VECTOR_COPROC_ERR           0x10

#define INT_VECTOR_SYS_CALL             0x90

#define INT_VECTOR_IRQ0                 0x20
#define INT_VECTOR_IRQ8                 0x28

// 8253/8254 PIT
#define TIMER0 0x40
#define TIMER_MODE 0x43
#define RATE_GENERATOR 0x34

#define TIMER_FREQ 1193182L
#define TIMER_HZ 100

/* VGA */
#define CRTC_ADDR_REG   0x3D4   /* CRT Controller Registers - Addr Register */
#define CRTC_DATA_REG   0x3D5   /* CRT Controller Registers - Data Register */
#define START_ADDR_H    0xC     /* reg index of video mem start addr (MSB) */
#define START_ADDR_L    0xD     /* reg index of video mem start addr (LSB) */
#define CURSOR_H        0xE     /* reg index of cursor position (MSB) */
#define CURSOR_L        0xF     /* reg index of cursor position (LSB) */
#define V_MEM_BASE      0xB8000 /* base of color video memory */
#define V_MEM_SIZE      0x8000  /* 32K: B8000H -> BFFFFH */

// Virtual address to physical address
#define vir2phy(seg_base, vaddr) \
    ((uint32)(((uint32)seg_base) + (uint32)(vaddr)))

// Some global variables used by other files
uint8 gdt_ptr[DT_PTR_LEN];            // gdt_ptr is set in kernel.asm
Descriptor gdt[GDT_SIZE];

uint8 idt_ptr[DT_PTR_LEN];           // idt_ptr is set in kernel.asm
Gate idt[IDT_SIZE];

Tss tss;
char task_stack[1];

uint32 reenter_cnt;

irq_handler irq_table[IRQ_NUM];

system_call sys_call_table[SYS_CALL_NUM];
int ticks;

#endif /* _PROTECT_H_ */
