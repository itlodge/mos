#include "type.h"
#include "protect.h"

extern void
disp_int(int n);

extern void
disp_str(const char *str);

extern void
out_byte(uint16 port, uint8 value);

extern void
disable_irq(int irq);

// External variables
extern irq_handler irq_table[IRQ_NUM];

void
spurious_irq(int irq);

void
init_8259A()
{
    // ICW1, Master
    out_byte(INT_M_CTL, 0x11);

    // ICW1, Slave
    out_byte(INT_S_CTL, 0x11);

    // ICW2, Master entry point 0x20
    out_byte(INT_M_CTLMASK, INT_VECTOR_IRQ0);

    // ICW2, Slave entry point 0x28
    out_byte(INT_S_CTLMASK, INT_VECTOR_IRQ8);

    // ICW3, Master
    out_byte(INT_M_CTLMASK, 0x4);

    // ICW3, Slave
    out_byte(INT_S_CTLMASK, 0x2);

    // ICW4, Master
    out_byte(INT_M_CTLMASK, 0x1);

    // ICW4, Slave
    out_byte(INT_S_CTLMASK, 0x1);

    // OCW1, Master
    out_byte(INT_M_CTLMASK, 0xFF);  // Trigger clock interrupt

    // OCW1, Slave
    out_byte(INT_S_CTLMASK, 0xFF);

    for (int i = 0; i < IRQ_NUM; ++i) {
        irq_table[i] = spurious_irq;
    }
}

void
spurious_irq(int irq)
{
    disp_str("spurious_irq: ");
    disp_int(irq);
    disp_str("\n");
}

void
put_irq_handler(int irq, irq_handler handler)
{
    disable_irq(irq);
    irq_table[irq] = handler;
}
