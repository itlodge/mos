#include "protect.h"
#include "process.h"

#define SCREEN_WIDTH 80

extern int disp_pos;
extern Gate idt[IDT_SIZE];
extern Process *proc_ready;
extern Process proc_list[PROCESS_NUM];
                 
extern void
init_8259A();

extern int
sys_get_ticks();

// External functions from ASM
extern void
disp_int(int n);

extern void
disp_str(const char *str);

extern void
disp_color_str(const char *str, int color);

extern void
memset(void *dst, char ch, int size);


// From kernel.asm
void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void page_fault();
void copr_error();

void sys_call();

void hwint00();
void hwint01();
void hwint02();
void hwint03();
void hwint04();
void hwint05();
void hwint06();
void hwint07();
void hwint08();
void hwint09();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();

static void
init_idt_vector(uint8 vector, uint8 vector_type, int_handler handler,
              uint8 privilege);

static void
init_desc(Descriptor *desc, uint32 base, uint32 limit, uint16 attr);

uint32
seg2phy(uint16 segment);

void
init_protect()
{
    init_8259A();

    init_idt_vector(INT_VECTOR_DIVIDE, DA_386IGate,
                  divide_error, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_DEBUG, DA_386IGate,
                  single_step_exception, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_NMI, DA_386IGate,
                  nmi, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_BREAKPOINT, DA_386IGate,
                  breakpoint_exception, PRIVILEGE_USER);

    init_idt_vector(INT_VECTOR_OVERFLOW, DA_386IGate,
                  overflow, PRIVILEGE_USER);

    init_idt_vector(INT_VECTOR_BOUNDS, DA_386IGate,
                  bounds_check, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_INVAL_OP, DA_386IGate,
                  inval_opcode, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_COPROC_NOT, DA_386IGate,
                  copr_not_available, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_DOUBLE_FAULT, DA_386IGate,
                  double_fault, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_COPROC_SEG, DA_386IGate,
                  copr_seg_overrun, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_INVAL_TSS, DA_386IGate,
                  inval_tss, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_SEG_NOT, DA_386IGate,
                  segment_not_present, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_STACK_FAULT, DA_386IGate,
                  stack_exception, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_PROTECTION, DA_386IGate,
                  general_protection, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_PAGE_FAULT, DA_386IGate,
                  page_fault, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_COPROC_ERR, DA_386IGate,
                  copr_error, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ0 + 0, DA_386IGate,
                  hwint00, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ0 + 1, DA_386IGate,
                  hwint01, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ0 + 2, DA_386IGate,
                  hwint02, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ0 + 3, DA_386IGate,
                  hwint03, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ0 + 4, DA_386IGate,
                  hwint04, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ0 + 5, DA_386IGate,
                  hwint05, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ0 + 6, DA_386IGate,
                  hwint06, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ0 + 7, DA_386IGate,
                  hwint07, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ8 + 0, DA_386IGate,
                  hwint08, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ8 + 1, DA_386IGate,
                  hwint09, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ8 + 2, DA_386IGate,
                  hwint10, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ8 + 3, DA_386IGate,
                  hwint11, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ8 + 4, DA_386IGate,
                  hwint12, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ8 + 5, DA_386IGate,
                  hwint13, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ8 + 6, DA_386IGate,
                  hwint14, PRIVILEGE_KRNL);

    init_idt_vector(INT_VECTOR_IRQ8 + 7, DA_386IGate,
                  hwint15, PRIVILEGE_KRNL);

    // System call
    init_idt_vector(INT_VECTOR_SYS_CALL, DA_386IGate,
                    sys_call, PRIVILEGE_USER);
    sys_call_table[0] = sys_get_ticks;
    
    // Put tss into GDT
    memset(&tss, 0, sizeof(tss));
    tss.ss0 = SELECTOR_KERNEL_DS;
    init_desc(&gdt[INDEX_TSS],
              vir2phy(seg2phy(SELECTOR_KERNEL_DS), &tss),
              sizeof(tss) - 1, DA_386TSS);
    tss.iobase = sizeof(tss);

    // Put LDT into GDT
    Process *p = proc_list;
    uint16 selector_ldt = INDEX_LDT_FIRST << 3;
    for (int i = 0; i < PROCESS_NUM; ++i) {
        init_desc(&gdt[selector_ldt >> 3],
                  vir2phy(seg2phy(SELECTOR_KERNEL_DS), proc_list[i].ldts),
                  LDT_SIZE * sizeof(Descriptor) - 1, DA_LDT);
        ++p;
        selector_ldt += (1 << 3);
    }
}

static void
init_idt_vector(uint8 vector, uint8 vector_type, int_handler handler,
              uint8 privilege)
{
    Gate *gate = &idt[vector];
    uint32 base = (uint32)handler;

    gate->offset_low = base & 0xFFFF;
    gate->selector = SELECTOR_KERNEL_CS;
    gate->dw_cnt = 0;
    gate->attr = vector_type | (privilege << 5);
    gate->offset_high = (base >> 16) & 0xFFFF;
}

static void
init_desc(Descriptor *desc, uint32 base, uint32 limit, uint16 attr)
{
    desc->limit_low = limit & 0x0FFFF;
    desc->base_low = base & 0x0FFFF;
    desc->base_mid = (base >> 16) & 0x0FF;
    desc->attr1 = attr & 0xFF;
    desc->limit_high_attr2 = ((limit >> 16) & 0x0F) | ((attr >> 8) & 0xF0);
    desc->base_high = (base >> 24) & 0x0FF;
}

void
exception_handler(int vec_num, int err_code, int eip, int cs, int eflags)
{
    int text_color = 0x74; // Red on Gray

    const char *err_msg[] = {
        "#DE Divide Error",
        "#DB RESERVED",
        "--  NMI Interrupt",
        "#BP Breakpoint",
        "#OF Overflow",
        "#BR BOUND Range Exceeded",
        "#UD Invalid Opcode (Undefined Opcode)",
        "#NM Device Not Available (No Math Coprocessor)",
        "#DF Double Fault",
        "    Coprocessor Segment Overrun (reserved)",
        "#TS Invalid TSS",
        "#NP Segment Not Present",
        "#SS Stack-Segment Fault",
        "#GP General Protection",
        "#PF Page Fault",
        "--  (Intel reserved. Do not use.)",
        "#MF x87 FPU Floating-Point Error (Math Fault)",
        "#AC Alignment Check",
        "#MC Machine Check",
        "#XF SIMD Floating-Point Exception"
    };

    disp_pos = 0;
    for (int i = 0; i < SCREEN_WIDTH * 5; ++i) {
        disp_str(" ");
    }
    disp_pos = 0;

    disp_color_str("Exception --> ", text_color);
    disp_color_str(err_msg[vec_num], text_color);
    disp_color_str("\n\n", text_color);
    disp_color_str("EFLAGS:", text_color);
    disp_int(eflags);
    disp_color_str("\n\n", text_color);
    disp_color_str("CS:", text_color);
    disp_int(cs);
    disp_color_str("\n\n", text_color);
    disp_color_str("EIP:", text_color);
    disp_int(eip);
    disp_color_str("\n\n", text_color);
    
    if (err_code != 0xFFFFFFFF) {
        disp_color_str("Error code:", text_color);
        disp_int(err_code);
        disp_color_str("\n\n", text_color);     
    }
}

uint32
seg2phy(uint16 segment)
{
    Descriptor *p = &gdt[segment >> 3];

    return (p->base_high << 24) | (p->base_mid << 16) | (p->base_low);
}

