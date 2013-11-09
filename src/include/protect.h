#ifndef _PROTECT_H_
#define _PROTECT_H_

#include "type.h"

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

// Descriptor type
#define DA_386IGate             0x8E    // 386 Interrupt

// Selectors
#define SELECTOR_DUMMY             0            
#define SELECTOR_FLAT_C         0x08            
#define SELECTOR_FLAT_RW        0x10            
#define SELECTOR_VIDEO          (0x18+3)        

#define SELECTOR_KERNEL_CS      SELECTOR_FLAT_C
#define SELECTOR_KERNEL_DS      SELECTOR_FLAT_RW

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

#define INT_VECTOR_IRQ0                 0x20
#define INT_VECTOR_IRQ8                 0x28


#endif /* _PROTECT_H_ */
