#include "type.h"
#include "const.h"
#include "protect.h"
#include "extern.h"

// Global variables
int disp_pos = 0;

// Extern
extern void
init_protect();

void
cstart()
{
    disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    // Change GDT
    memcpy(&gdt, (void *)(*(uint32 *)(&gdt_ptr[2])),
           *((uint16 *)(&gdt_ptr[0])) + 1);
    
    uint16 *gdt_limit = (uint16 *)(&gdt_ptr[0]);
    uint32 *gdt_base = (uint32 *)(&gdt_ptr[2]);
    
    *gdt_limit = GDT_SIZE * sizeof(Descriptor) - 1;
    *gdt_base = (uint32)&gdt;

    // Initialize IDT
    uint16 *idt_limit = (uint16 *)(&idt_ptr[0]);
    uint32 *idt_base = (uint32 *)(&idt_ptr[2]);
    *idt_limit = IDT_SIZE * sizeof(Gate) - 1;
    *idt_base = (uint32)&idt;

    init_protect();
}

    
