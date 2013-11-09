

extern disp_pos
        
[section .text]

global  memcpy
        
;; -------------------------------------------------
;; void * memcpy(void *dest, void *src, int size);
;; --------------------------------------------------
memcpy:
        push    ebp
        mov     ebp, esp

        push    esi
        push    edi
        push    ecx

        mov     edi, [ebp + 8]  ; Destination
        mov     eax, edi
        mov     esi, [ebp + 12] ; Source
        mov     ecx, [ebp + 16] ; Size
.begin:
        cmp     ecx, 0
        jz      .end

        mov     al, [ds:esi]
        inc     esi

        mov     byte [es:edi], al
        inc     edi

        dec     ecx
        jmp     .begin
.end:
        ;; mov     eax, [ebp + 8]  ; Return value is stored in EAX
        ;;                         ; Return value is the Destination
        pop     ecx
        pop     edi
        pop     esi
        mov     esp, ebp
        pop     ebp
        ret

;; ---------------------------------------------
;; void disp_str(const char *str);
;; ---------------------------------------------
global  disp_str
        
disp_str:
        push    ebp
        mov     ebp, esp
        push    ebx
        push    esi
        push    edi

        mov     esi, [ebp + 8]  ; Source, the string
        mov     edi, [disp_pos]  ; Destination, the position to display
        mov     ah, 0Fh
.begin:
        lodsb
        test    al, al          ; If AL dosen't changed, there is nothing to
        jz      .end            ; load, finished.
        cmp     al, 0Ah         ; Is '\n' ?
        jnz     .show
        push    eax
        mov     eax, edi        ; EAX, the position to be changed
        mov     bl, 160         ; 2 * 80 = 160
        div     bl,             ; 8-bit division, AL will hold the line num
        and     eax, 0FFh       ; Now EAX only contains AL
        inc     eax             ; To the next line

        mov     bl, 160
        mul     bl              ; The first position of the next line

        mov     edi, eax        ; Now EDI is changed
        pop     eax
        jmp     .begin

.show:
        mov     [gs:edi], ax    ; Output the video memory
        add     edi, 2          ; One time one byte(two position)
        jmp     .begin
.end:
        mov     [disp_pos], edi  ; Now the display position has been changed
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp
        ret

;; ---------------------------------------------
;; void disp_color_str(const char *str, int color);
;; ---------------------------------------------
global  disp_color_str
        
disp_color_str:
        push    ebp
        mov     ebp, esp
        push    ebx
        push    esi
        push    edi

        mov     esi, [ebp + 8]  ; Source, the string
        mov     edi, [disp_pos]  ; Destination, the position to display
        mov     ah, [ebp + 12]   ; Color
.begin:
        lodsb
        test    al, al          ; If AL dosen't changed, there is nothing to
        jz      .end            ; load, finished.
        cmp     al, 0Ah         ; Is '\n' ?
        jnz     .show
        push    eax
        mov     eax, edi        ; EAX, the position to be changed
        mov     bl, 160         ; 2 * 80 = 160
        div     bl             ; 8-bit division, AL will hold the line num
        and     eax, 0FFh       ; Now EAX only contains AL
        inc     eax             ; To the next line

        mov     bl, 160
        mul     bl              ; The first position of the next line

        mov     edi, eax        ; Now EDI is changed
        pop     eax
        jmp     .begin

.show:
        mov     [gs:edi], ax    ; Output the video memory
        add     edi, 2          ; One time one byte(two position)
        jmp     .begin
.end:
        mov     [disp_pos], edi  ; Now the display position has been changed
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp
        ret
        
;; -------------------------------------------
;; void out_byte(uint16 port, uint8 value);
;; -------------------------------------------
global out_byte
        
out_byte:
        mov     edx, [esp + 4]  ; Port
        mov     al, [esp + 8]   ; Value
        out     dx, al
        nop
        nop
        ret

;; -------------------------------------------
;; uint8 in_byte(uint16 port);
;; -------------------------------------------
global int_byte
        
int_byte:
        mov     edx, [esp + 4]  ; Port
        xor     eax, eax
        in      al, dx
        nop
        nop
        ret
        
        
