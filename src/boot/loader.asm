;; Loader

org     0100h                   ; .COM file
jmp     start

%include "fat12hdr.inc"
%include "pm.inc"
        
STACK_BASE              equ     0100h
;; Loader
LOADER_BASE             equ     09000h
LOADER_OFFSET           equ     0100h
LOADER_PHY_ADDR_BASE    equ     LOADER_BASE * 10h
;; Kernel
KERNEL_BASE             equ     08000h
KERNEL_OFFSET           equ     0h
KERNEL_PHY_ADDR_BASE    equ     KERNEL_BASE * 10h
KERNEL_PHY_ADDR_ENTRY   equ     030400h ; ld -Ttext 0x30400
;; Paging
PAGE_DIR_BASE           equ     200000h
PAGE_TBL_BASE           equ     201000h
        
;; All descriptor
GDT:            Descriptor 0,       0,       0
DESC_FLAT_C:    Descriptor 0,       0fffffh, DA_CR | DA_32 | DA_LIMIT_4K
DESC_FLAT_RW:   Descriptor 0,       0fffffh, DA_DRW | DA_32 | DA_LIMIT_4K
DESC_VIDEO:     Descriptor 0B8000h, 0ffffh, DA_DRW | DA_DPL3

GDT_LEN         equ     $ - GDT
GdtPtr          dw      GDT_LEN - 1
                dd      LOADER_PHY_ADDR_BASE + GDT

;; All selector
SELECTOR_FLAT_C         equ     DESC_FLAT_C - GDT
SELECTOR_FLAT_RW        equ     DESC_FLAT_RW - GDT
SELECTOR_VIDEO          equ     DESC_VIDEO - GDT + SA_RPL3
        
start:  
        mov     ax, cs
        mov     ds, ax
        mov     es, ax
        mov     ss, ax
        mov     sp, STACK_BASE

        mov     dh, 0           ; "Loading  "
        call    show_str

        mov     ebx, 0          ; EBX is continuation value
        mov     di, _MemChkBuf  ; ES:DI -> ARDS
.mem_begin:
        mov     eax, 0E820h     ; EAX must be 0E820h
        mov     ecx, 20         ; Size of ARDS
        mov     edx, 0534D4150h ; 'SMAP'
        int     15h
        jc      .mem_fail       ; If error, CF=1
        add     di, 20
        inc     dword [_McrNum] ; Number of ARDS
        cmp     ebx, 0
        jne     .mem_begin
        jmp     .mem_end        ; If EBX is zero, finished.
.mem_fail:
        mov     dword [_McrNum], 0
.mem_end:
        nop

        ;; Search for KERNEL.BIN
        mov     word [SectorNum], SECTOR_NUM_OF_ROOT_DIR

        ;; Reset the floppy
        xor     ah, ah
        xor     dl, dl
        int     13h

search_root_dir:
        cmp     word [RootDirSize], 0
        jz      kernel_not_found
        dec     word [RootDirSize]
        mov     ax, KERNEL_BASE
        mov     es, ax
        mov     bx, KERNEL_OFFSET
        mov     ax, [SectorNum]
        mov     cl, 1
        call    read_sector

        mov     si, KernelName
        mov     di, KERNEL_OFFSET
        cld
        mov     dx, 10h
search_kernel:  
        cmp     dx, 0
        jz      next_sector
        dec     dx
        mov     cx, 11
cmp_file:
        cmp     cx, 0
        jz      kernel_found
        dec     cx
        lodsb
        cmp     al, byte [es:di]
        jz      cmp_again
        jmp     different
cmp_again:
        inc     di
        jmp     cmp_file
different:
        and     di, 0FFE0h
        add     di, 20h
        mov     si, KernelName
        jmp     search_kernel
next_sector:
        add     word [SectorNum], 1
        jmp     search_root_dir
kernel_not_found:
        mov     dh, 2
        call    show_str
        jmp     $
kernel_found:
        mov     ax, ROOT_DIR_SECTORS
        and     di, 0FFF0h

        push    eax
        mov     eax, [es:di + 01Ch]
        mov     dword [KernelSize], eax
        pop     eax

        add     di, 01Ah
        mov     cx, word[es:di]
        push    cx
        add     cx, ax
        add     cx, DELTA_SECTOR_NUM
        mov     ax, KERNEL_BASE
        mov     es, ax
        mov     bx, KERNEL_OFFSET
        mov     ax, cx
loading_kernel:
        push    ax
        push    bx
        mov     ah, 0Eh
        mov     al, '.'
        mov     bl, 0Fh
        int     10h
        pop     bx
        pop     ax

        mov     cl, 1
        call    read_sector
        pop     ax
        call    get_FAT_entry
        cmp     ax, 0FFFh
        jz      kernel_loaded
        push    ax
        mov     dx, ROOT_DIR_SECTORS
        add     ax, dx
        add     ax, DELTA_SECTOR_NUM
        add     bx, [BPB_BytsPerSec]
        jmp     loading_kernel
kernel_loaded:
        call    kill_motor
        mov     dh, 1
        call    show_str

        lgdt    [GdtPtr]        ; Load GDT
        cli                     ; Close interrupt
        in      al, 92h         ; Open A20
        or      al, 00000010b
        out     92h, al

        mov     eax, cr0        ; Set PE bit
        or      eax, 1
        mov     cr0, eax
        
        jmp     dword SELECTOR_FLAT_C:(LOADER_PHY_ADDR_BASE + pm_start)
        
RootDirSize     dw      ROOT_DIR_SECTORS
SectorNum       dw      0
IsOdd           db      0
KernelSize      dd      0
        
KernelName      db      "KERNEL  BIN", 0
MSG_LEN         equ     9
LoadMsg         db      "Loading  "
ReadyMsg        db      "Ready.   "
NoKernelMsg     db      "No Kernel"
        
;; -----------------------------------------------
;; show a string, DH is
;; 0, "Loading  "
;; 1, "Ready.   "
;; 2, "No Kernel"
show_str:
        mov     ax, MSG_LEN
        mul     dh
        add     ax, LoadMsg
        mov     bp, ax
        mov     ax, ds
        mov     es, ax
        mov     cx, MSG_LEN
        mov     ax, 01301h
        mov     bx, 0007h
        mov     dl, 0
        add     dh, 3
        int     10h
        ret
        
;; ------------------------------------------------
        
;; ------------------
;; Read CL sectors
read_sector:
        push    bp
        mov     bp, sp
        sub     esp, 2

        mov     byte [bp - 2], cl
        push    bx
        mov     bl, [BPB_SecPerTrk]
        div     bl
        inc     ah
        mov     cl, ah
        mov     dh, al
        shr     al, 1
        mov     ch, al
        and     dh, 1
        pop     bx
        mov     dl, [BS_DrvNum]
.reading:
        mov     ah, 2
        mov     al, byte [bp - 2]
        int     13h
        jc      .reading

        add     esp, 2
        pop     bp

        ret

;; -------------------------
;; Get an entry from FAT
get_FAT_entry:
        push    es
        push    bx
        push    ax
        mov     ax, KERNEL_BASE
        sub     ax, 0100h
        mov     es, ax
        pop     ax
        mov     byte [IsOdd], 0
        mov     bx, 3
        mul     bx
        mov     bx, 2
        div     bx
        cmp     dx, 0
        jz      is_even
        mov     byte [IsOdd], 1
is_even:
        xor     dx, dx
        mov     bx, [BPB_BytsPerSec]
        div     bx
        push    dx
        mov     bx, 0
        add     ax, SECTOR_NUM_OF_FAT1
        mov     cl, 2
        call    read_sector
        pop     dx
        add     bx, dx
        mov     ax, [es:bx]
        cmp     byte [IsOdd], 1
        jnz     is_even_again
        shr     ax, 4
is_even_again:
        and     ax, 0FFFh

        pop     bx
        pop     es
        ret

;; --------------------------
;; Kill the floppy motor
kill_motor:
        push    dx
        mov     dx, 03F2h
        mov     al, 0
        out     dx, al
        pop     dx
        ret
        
[section .s32]
align   32
[bits 32]
        
pm_start:
        mov     ax, SELECTOR_VIDEO
        mov     gs, ax

        mov     ax, SELECTOR_FLAT_RW
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     ss, ax
        mov     esp, TOP_OF_STACK

        push    MemChkTitle
        call    display_str
        add     esp, 4

        call    disp_mem_info
        call    setup_paging
        call    init_kernel
        jmp     SELECTOR_FLAT_C:KERNEL_PHY_ADDR_ENTRY
        ;; jmp     $

%include "lib.inc"              
        
disp_mem_info:
        push    esi
        push    edi
        push    ecx

        mov     esi, MemChkBuf
        mov     ecx, [McrNum]
.begin:
        mov     edx, 5          ; 5 members in ARDS
        mov     edi, ARD
.show:
        push    dword [esi]
        call    display_int
        pop     eax             ; Why?
        stosd                   ; Store double word in EAX to ES:EDI
        add     esi, 4
        dec     edx             ; Next member
        cmp     edx, 0
        jnz     .show           ; Continue showing
        call    display_return  ; printf("\n");
        cmp     dword [Type], 1
        jne     .again          ; Memory cannot be used by OS
        mov     eax, [BaseAddrLow]
        add     eax, [LengthLow]
        cmp     eax, [MemSize]
        jb      .again          ; BaseAddrLow + LengthLow < MemSize
        mov     [MemSize], eax
.again:
        loop    .begin

        call    display_return

        push    RAMSizeStr
        call    display_str
        add     esp, 4
        
        push    dword [MemSize]
        call    display_int
        add     esp, 4

        pop     ecx
        pop     edi
        pop     esi
        ret

setup_paging:
        xor     edx, edx
        mov     eax, [MemSize]
        mov     ebx, 400000h    ; 400000h = 4 * 100000 = 4096 * 1024 = 4M

        div     ebx
        mov     ecx, eax        ; EAX is the quotient -> the number of PDE
        test    edx, edx
        jz      .no_reminder
        inc     ecx             ; Need one more PDE
.no_reminder:
        push    ecx

        mov     ax, SELECTOR_FLAT_RW
        mov     es, ax
        mov     edi, PAGE_DIR_BASE
        xor     eax, eax
        mov     eax, PAGE_TBL_BASE | PG_P | PG_USU | PG_RWW

.PDE_begin:
        stosd                   ; Store EAX into ES:EDI
        add     eax, 4096
        loop    .PDE_begin

        pop     eax             ; Why?
        mov     ebx, 1024
        mul     ebx
        mov     ecx, eax        ; num_PTE = num_PDE * 1024
        mov     edi, PAGE_TBL_BASE
        xor     eax, eax
        mov     eax, PG_P | PG_USU | PG_RWW
        
.PTE_begin:
        stosd
        add     eax, 4096
        loop    .PTE_begin

        mov     eax, PAGE_DIR_BASE
        mov     cr3, eax        ; CR3 to store page directory head
        mov     eax, cr0
        or      eax, 80000000h
        mov     cr0, eax
        jmp     short .end
.end:
        nop
        ret

;; -------------------
;; Initialize the kernel
init_kernel:
        xor     esi, esi
        mov     cx, word [KERNEL_PHY_ADDR_BASE + 2Ch] ; e_phnum
        movzx   ecx, cx
        mov     esi, [KERNEL_PHY_ADDR_BASE + 1Ch] ; e_phoff
        add     esi, KERNEL_PHY_ADDR_BASE       ; ESI is now the program
                                                ; header address
.begin:
        mov     eax, [esi + 0]  
        cmp     eax, 0
        jz      .nothing
        push    dword [esi + 010h] ; Size
        mov     eax, [esi + 04h]
        add     eax, KERNEL_PHY_ADDR_BASE 
        push    eax                     ; Source
        push    dword [esi + 08h]       ; Destination
        call    mem_cpy
        add     esp, 12
.nothing:
        add     esi, 020h       ; Each entry is 32 bytes
        dec     ecx
        jnz     .begin
        
        ret
        
[section .data1]
align   32

;; In real mode
_MemChkTitle:   db      "BaseLow   BaseHigh LengthLow LengthHigh Type", 0Ah, 0
_RAMSizeStr:    db      "RAM size:", 0
_Return:        db      0Ah, 0
        
_McrNum:        dd      0                 ; Memory check result
_DispPos:       dd      (80 * 6 + 0) * 2
_MemSize:       dd      0

_ARD:                                     ; Address range descriptor structure
_BaseAddrLow:   dd      0
_BaseAddrHigh:  dd      0
_LengthLow:     dd      0
_LengthHigh:    dd      0
_Type:          dd      0
_MemChkBuf:     times   256     db      0 ; Memory check buffer
        
        
;; In protected mode
MemChkTitle:   equ      LOADER_PHY_ADDR_BASE + _MemChkTitle
RAMSizeStr:    equ      LOADER_PHY_ADDR_BASE + _RAMSizeStr
Return:        equ      LOADER_PHY_ADDR_BASE + _Return
DispPos:       equ      LOADER_PHY_ADDR_BASE + _DispPos
MemSize:       equ      LOADER_PHY_ADDR_BASE + _MemSize
McrNum:        equ      LOADER_PHY_ADDR_BASE + _McrNum
        
ARD:           equ      LOADER_PHY_ADDR_BASE + _ARD
BaseAddrLow:   equ      LOADER_PHY_ADDR_BASE + _BaseAddrLow
BaseAddrHigh:  equ      LOADER_PHY_ADDR_BASE + _BaseAddrHigh
LengthLow:     equ      LOADER_PHY_ADDR_BASE + _LengthLow
LengthHigh:    equ      LOADER_PHY_ADDR_BASE + _LengthHigh
Type:          equ      LOADER_PHY_ADDR_BASE + _Type
MemChkBuf:     equ      LOADER_PHY_ADDR_BASE + _MemChkBuf

[section .stack]
STACK_SPACE:    times   1000h    db      0
TOP_OF_STACK    equ     LOADER_PHY_ADDR_BASE + $
        
