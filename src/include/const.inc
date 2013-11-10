STACK_BASE      equ     0
GS_REG          equ     STACK_BASE
FS_REG          equ     GS_REG           + 4
ES_REG          equ     FS_REG           + 4
DS_REG          equ     ES_REG           + 4
EDI_REG         equ     DS_REG           + 4
ESI_REG         equ     EDI_REG          + 4
EBP_REG         equ     ESI_REG          + 4
KERNEL_ESP_REG  equ     EBP_REG          + 4
EBX_REG         equ     KERNEL_ESP_REG   + 4
EDX_REG         equ     EBX_REG          + 4
ECX_REG         equ     EDX_REG          + 4
EAX_REG         equ     ECX_REG          + 4
RETADDR         equ     EAX_REG          + 4
EIP_REG         equ     RETADDR          + 4
CS_REG          equ     EIP_REG          + 4
EFLAGS_REG      equ     CS_REG           + 4
ESP_REG         equ     EFLAGS_REG       + 4
SS_REG          equ     ESP_REG          + 4
STACK_TOP       equ     SS_REG           + 4
LDT_SEL         equ     STACK_TOP
LDT             equ     LDT_SEL          + 4

TSS3_S_SP0              equ     4 ; Refer to the definition of Tss, it's ESP0
        
SELECTOR_KERNEL_CS      equ     8
SELECTOR_TSS            equ     0x20

;; Interrupt
INT_M_CTL       equ     0x20 ; I/O port for interrupt controller        <Master>
INT_M_CTLMASK   equ     0x21 ; setting bits in this port disables ints  <Master>
INT_S_CTL       equ     0xA0 ; I/O port for second interrupt controller <Slave>
INT_S_CTLMASK   equ     0xA1 ; setting bits in this port disables ints  <Slave>

EOI             equ     0x20

