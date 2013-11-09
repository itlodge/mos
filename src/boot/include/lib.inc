;; ---------
;; Display digit in AL
;; [Note] A global variable named DispPos must be used as the display
;;        position of the digit in AL.
        
display_al:
        push    ecx
        push    edx
        push    edi

        mov     edi, [DispPos]  ; The display position.
        mov     ah, 0Fh         ; 0000b: Black background, 1111b: White color.
        mov     dl, al          ; DL store the backup.
        shr     al, 4           ; First output the high 4 bits
        mov     ecx, 2          ; Loop 2 times
.begin:
        and     al, 01111b
        cmp     al, 9
        ja      .letter
        add     al, '0'
        jmp     .show
.letter:
        sub     al, 0Ah
        add     al, 'A'         ; AL may be A, B, C, D, E, F
.show:
        mov     [gs:edi], ax    ; Output
        add     edi, 2          ; Next position

        mov     al, dl          ; Get the backup
        loop    .begin

        mov     [DispPos], edi  ; The display position has been changed

        pop     edi
        pop     edx
        pop     ecx
        ret

;; ------------------
;; Show an integer, only for 32 bits
;; [Note] A global variable named DispPos must be used as the display
;;        position of the integer.
display_int:
        mov     eax, [esp + 4]  ; The integer to show

        ;; Display the first 8 bits
        shr     eax, 24
        call    display_al

        ;; Dispaly the second 8 bits
        mov     eax, [esp + 4]
        shr     eax, 16
        call    display_al

        ;; Display the third 8 bits
        mov     eax, [esp + 4]
        shr     eax, 8
        call    display_al

        ;; Display the last 8 bits
        mov     eax, [esp + 4]
        call    display_al

        ;; Display a 'h' character
        mov     ah, 07h         ; 0000b: Black background, 0111b: gray color
        mov     al, 'h'
        push    edi
        mov     edi, [DispPos]
        mov     [gs:edi], ax
        add     edi, 4
        mov     [DispPos], edi
        pop     edi

        ret
        
;; ------------------------------
;; Display a string
;; [Note] A global variable named DispPos must be used as the display
;;        position of the string.
display_str:
        push    ebp
        mov     ebp, esp
        push    ebx
        push    esi
        push    edi

        mov     esi, [ebp + 8]  ; Source, the string
        mov     edi, [DispPos]  ; Destination, the position to display
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
        div     bl              ; 8-bit division, AL will hold the line num
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
        mov     [DispPos], edi  ; Now the display position has been changed
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp
        ret

;; --------------------
;; Display '\n'
;; [Note] A global variable named DispPos must be used as the display
;;        position. A global variable named Return must be used as the
;;        address of the '\n\0' string
display_return:
        push    Return
        call    display_str
        add     esp, 4

        ret

;; --------------------------------
;; memory copy
;; The first parameter is the Destination,
;; The second is the Source
;; The third is the Size
mem_cpy:
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
        mov     eax, [ebp + 8]  ; Return value is stored in EAX
                                ; Return value is the Destination
        pop     ecx
        pop     edi
        pop     esi
        mov     esp, ebp
        pop     ebp
        ret


;; mem_cpy:
;;         push    ebp
;;         mov     ebp, esp
;;         push    esi
;;         push    edi

;;         mov     edi, [ebp + 8]
;;         mov     eax, edi
;;         mov     esi, [ebp + 12]
;;         mov     ecx, [ebp + 16]
;;         rep     movsb

;;         pop     edi
;;         pop     esi
;;         pop     ebp
;;         ret
