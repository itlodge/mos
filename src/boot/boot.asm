;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; MOS, My first operating system ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

org     07c00h             ; Original entry point is 7c00h
jmp     short start
nop
     
;; Constants
;; ===================================
STACK_BASE              equ     07c00h
LOADER_BASE             equ     09000h
LOADER_OFFSET           equ     0100h
;; ===================================

%include "fat12hdr.inc"
        
start:  
        mov     ax, cs          
        mov     ds, ax          ; Let ds, es and ss point to the same place
        mov     es, ax          ; of cs.
        mov     ss, ax
        mov     sp, STACK_BASE

        mov     ax, 0600h       ; AH=06 means scrolling, AL is number of lines
                                ; to scroll. 0 means the whole page.
        mov     bx, 0700h       ; BL=07h means white on black
        mov     cx, 0           ; left, (0, 0)
        mov     dx, 0184fh      ; right, (0x18, 0x4f) -> (24, 79)
        int     10h             ; 10h interrupt is for screen

        mov     dh, 0           ; Show "Booting  "
        call    show_str
        
        ;; Reset the floppy
        xor     ah, ah
        xor     dl, dl
        int     13h

        ;; Search for loader.bin
        mov     word [SectorNum], SECTOR_NUM_OF_ROOT_DIR ; 19
search_root_dir:
        cmp     word [RootDirSize], 0
        jz      loader_not_found
        dec     word [RootDirSize]
        mov     ax, LOADER_BASE
        mov     es, ax                  ; es -> LOADER_BASE
        mov     bx, LOADER_OFFSET       ; bx -> LOADER_OFFSET
        mov     ax, [SectorNum]
        mov     cl, 1                   ; Always read one sector
        call    read_sector

        mov     si, LoaderFileName      ; ds:si -> LoaderFileName
        mov     di, LOADER_OFFSET       ; es:di -> LOADER_BASE:LOADER_OFFSET
        cld                             ; Clear DF, adress from low to high
        mov     dx, 10h                 ; Why 10h?
search_loader:
        cmp     dx, 0
        jz      search_next_sector
        dec     dx
        mov     cx, 11
cmp_name:
        cmp     cx, 0
        jz      loader_found
        dec     cx
        lodsb                           ; Load byte from ds:si to al
        cmp     al, byte [es:di]
        jz      cmp_again
        jmp     name_different
cmp_again:
        inc     di
        jmp     cmp_name
name_different:
        and     di, 0FFE0h
        add     di, 20h
        mov     si, LoaderFileName
        jmp     search_loader
        
search_next_sector:
        add     word [SectorNum], 1
        jmp     search_root_dir
        
loader_not_found:
        mov     dh, 2           ; Show "No loader "
        call    show_str
        jmp     $
        
loader_found:
        mov     ax, ROOT_DIR_SECTORS
        and     di, 0FFE0h
        add     di, 01Ah
        mov     cx, word [es:di]
        push    cx
        add     cx, ax
        add     cx, DELTA_SECTOR_NUM
        mov     ax, LOADER_BASE
        mov     es, ax                  ; es -> LOADER_BASE
        mov     bx, LOADER_OFFSET       ; bx -> LOADER_OFFSET
        mov     ax, cx                  ; ax -> Sector number
loading_file:
        push    ax
        push    bx
        mov     ah, 0Eh
        mov     al, '.'         ; Output '.'
        mov     bl, 0Fh
        int     10h
        pop     bx
        pop     ax

        mov     cl, 1
        call    read_sector
        pop     ax
        call    get_FAT_entry
        cmp     ax, 0FFFh
        jz      file_loaded
        push    ax
        mov     dx, ROOT_DIR_SECTORS
        add     ax, dx
        add     ax, DELTA_SECTOR_NUM
        add     bx, [BPB_BytsPerSec]
        jmp     loading_file
file_loaded:
        mov     dh, 1
        call    show_str

        jmp     LOADER_BASE:LOADER_OFFSET

        
RootDirSize     dw      ROOT_DIR_SECTORS
SectorNum       dw      0
IsOdd           db      0
        
;; Variables
;; =======================================
LoaderFileName  db      "LOADER  BIN", 0
MSG_LEN         equ     9
BootMsg:        db      "Booting  " ; 0
ReadyMsg        db      "Ready.   " ; 1
NoLoaderMsg     db      "No Loader" ; 2
        
;; Show a string
;; -------------------------------------------
show_str:
        mov     ax, MSG_LEN
        mul     dh
        add     ax, BootMsg
        mov     bp, ax
        mov     ax, ds
        mov     es, ax          ; ES:BP -> string
        mov     cx, MSG_LEN
        mov     ax, 01301h      ; AH=13h, show string, AL=01h, cursor move
        mov     bx, 0007h       ; BH=00h, page 0, BL=07h, white on black
        mov     dl, 0
        int     10h
        ret
;; ----------------------------------------------

;; Read CL sectors into ES:BX
;; -----------------------------------
read_sector:
        push    bp
        mov     bp, sp
        sub     esp, 2

        mov     byte [bp - 2], cl
        push    bx
        mov     bl, [BPB_SecPerTrk]
        div     bl              ; AX / BL, quotient is AL, reminder is AH
        inc     ah
        mov     cl, ah
        mov     dh, al
        shr     al, 1
        mov     ch, al          ; CH -> track num
        and     dh, 1           ; DH -> head num
        pop     bx

        mov     dl, [BS_DrvNum] ; 0
.reading:
        mov     ah, 2           ; AH=2, read
        mov     al, byte [bp - 2] ; AL is the number of sectors to read
        int     13h               ; 13h interrupt is for disk
        jc      .reading          ; If error, CF=1

        add     esp, 2
        pop     bp
        ret
;; --------------------------------------------------

;; Get an entry from FAT
;; --------------------------------------
get_FAT_entry:
        push    es
        push    bx
        
        push    ax
        mov     ax, LOADER_BASE
        sub     ax, 0100h
        mov     es, ax
        pop     ax

        ;; Now ax is the ID of the sector
        mov     byte [IsOdd], 0
        mov     bx, 3
        mul     bx              ; AX = AX * 3
        mov     bx, 2
        div     bx              ; AX / 2, quotient is AX, reminder is DX
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
;; ---------------------------------------------------
        
times   510 - ($ - $$)  db      0       ; Fill zero for the rest space
dw      0xaa55                          ; The last two bytes must be 0xaa55
        
