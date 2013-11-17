
IDX_get_ticks           equ     0
IDX_write               equ     1
INT_VECTOR_SYS_CALL     equ     0x90



bits 32
[section .text]
global  get_ticks
get_ticks:
        mov     eax, IDX_get_ticks
        int     INT_VECTOR_SYS_CALL
        ret

global write
write:
        mov     eax, IDX_write
        mov     ebx, [esp + 4]
        mov     ecx, [esp + 8]
        int     INT_VECTOR_SYS_CALL
        ret

        
