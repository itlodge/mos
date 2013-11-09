#include "type.h"
#include "const.h"
#include "protect.h"

// Global variables
uint8 gdt_ptr[DT_PTR_LEN];            // gdt_ptr is set in kernel.asm
Descriptor gdt[GDT_SIZE];

uint8 idt_ptr[DT_PTR_LEN];           // idt_ptr is set in kernel.asm
Gate idt[IDT_SIZE];

int disp_pos = 0;

// Global functions
void *
memcpy(void *dest, void *src, uint32 size);

void
disp_str(const char *str);

// Extern
extern void
init_protect();

void
cstart()
{
    disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n---Begin---\n");

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

    disp_str("-----end-------\n");
}

    
