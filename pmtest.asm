%include "pm.inc"
	   
org   0100h
jmp LABEL_BEGIN
	   
[SECTION .gdt]
LABEL_GDT:		Descriptor	0   , 0,		0 
LABEL_DESC_NORMAL:	Descriptor	0   , 0ffffh, 		DA_DRW
LABEL_DESC_CODE32:	Descriptor	0   , SegCode32Len - 1, DA_C + DA_32   
LABEL_DESC_CODE16:	Descriptor	0   , 0ffffh, 		DA_C
LABEL_DESC_DATA:	Descriptor	0   , DATA_LEN - 1, 	DA_DRW
LABEL_DESC_STACK:	Descriptor	0   , TOP_OF_STACK, 	DA_DRWA + DA_32
LABEL_DESC_TEST:	Descriptor 0500000h , 0ffffh, 		DA_DRW   
LABEL_DESC_VIDEO:	Descriptor 0B8000h  , 0ffffh, 		DA_DRW
   
GdtLen 	equ 	$ - LABEL_GDT	; GDT length
GdtPtr 	dw 	GdtLen - 1	; GDT limit
	dd	0		; GDT base
;;; --------------------------------------------------
	
;;; Selecotr
SELECTOR_NORMAL equ LABEL_DESC_NORMAL - LABEL_GDT
SELECTOR_CODE32 equ LABEL_DESC_CODE32 - LABEL_GDT
SELECTOR_CODE16 equ LABEL_DESC_CODE16 - LABEL_GDT
SELECTOR_DATA   equ LABEL_DESC_DATA   - LABEL_GDT
SELECTOR_STACK  equ LABEL_DESC_STACK  - LABEL_GDT
SELECTOR_TEST   equ LABEL_DESC_TEST   - LABEL_GDT
SELECTOR_VIDEO  equ LABEL_DESC_VIDEO  - LABEL_GDT

[SECTION .data]
ALIGN 32
[BITS 32]
LABEL_DATA:
sp_rm		dw	0
pm_msg		db	"In Protect Mode now.", 0
PM_MSG_OFFSET	equ	pm_msg - $$
test_str	db	"abcd", 0
TEST_STR_OFFSET	equ	test_str - $$
DATA_LEN	equ	$ - LABEL_DATA
;;; ----------------------------------------------------
	
[SECTION .gs]
ALIGN 32
[BITS 32]
LABEL_STACK:
	times 512 db 0
TOP_OF_STACK	equ	$ - LABEL_STACK - 1
;;; ------------------------------------------------------
	
[SECTION .s16]
[BITS 16]
LABEL_BEGIN:	
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h

	mov	[LABEL_BACK_TO_REAL + 3], ax
	mov	[sp_rm], sp

	;; initialize 16 code segment
	mov	ax, cs
	movzx	eax, ax
	shl	eax, 4
	add	eax, LABEL_SEG_CODE16
	mov	word [LABEL_DESC_CODE16 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE16 + 4], al
	mov	byte [LABEL_DESC_CODE16 + 7], ah

	;; initialize 32 code segment
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE32
	mov	word [LABEL_DESC_CODE32 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE32 + 4], al
	mov	byte [LABEL_DESC_CODE32 + 7], ah

	;; initialize data segment
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_DATA
	mov	word [LABEL_DESC_DATA + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_DATA + 4], al
	mov	byte [LABEL_DESC_DATA + 7], ah

	;; initialize stack segment
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_STACK
	mov	word [LABEL_DESC_STACK + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_STACK + 4], al
	mov	byte [LABEL_DESC_STACK + 7], ah

	;; initialize Gdtr
	xor	 eax, eax
	mov	 ax, ds
	shl	 eax, 4
	add	 eax, LABEL_GDT
	mov	 dword [GdtPtr + 2], eax
	 
	lgdt	 [GdtPtr]
	 
	cli	 
	 
	in	 al, 92h
	or	 al, 00000010h
	out	 92h, al
	 
	mov	 eax, cr0
	or	 eax, 1
	mov	 cr0, eax
	 
	jmp	 dword SELECTOR_CODE32:0

LABEL_REAL_ENTRY:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov 	ss, ax

	mov	sp, [sp_rm]

	in	al, 92h
	and	al, 11111101b
	out	92h, al

	sti

	mov	ax, 4c00h
	int	21h
;;; -------------------------------------------------------------
	
[SECTION .s32]
[BITS 32]
LABEL_SEG_CODE32:
	mov	ax, SELECTOR_DATA
	mov	ds, ax
	mov	ax, SELECTOR_TEST
	mov	es, ax
	mov	ax, SELECTOR_VIDEO
	mov	gs, ax
	mov	ax, SELECTOR_STACK
	mov	ss, ax
	mov	esp, TOP_OF_STACK
	
	mov	ah, 0ch
	xor	esi, esi
	xor	edi, edi
	mov	esi, PM_MSG_OFFSET
	mov	edi, (80 * 10 + 0) * 2 ; 80 row 0 column
	cld

.start:
	lodsb
	test	al, al
	jz	.end
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.start
.end:
	call	DispReturn
	call	TestRead
	call	TestWrite
	call	TestRead

	jmp	SELECTOR_CODE16:0

;;; --------------------------------------------------------------

;;; ----------
;; TestRead
;;; ----------
TestRead:
	xor	esi, esi
	mov	ecx, 8
.loop:
	mov	al, [es:esi]
	call	DispAL
	inc	esi
	loop	.loop
	call	DispReturn
	ret
;;; -------------------------------

;;; -------
;; TestWrite
;;; -------
TestWrite:
	push	esi
	push	edi
	xor	esi, esi
	xor	edi, edi
	mov	esi, TEST_STR_OFFSET
	cld
.start:
	lodsb
	test	al, al
	jz	.end
	mov	[es:edi], al
	inc	edi
	jmp	.start
.end:
	pop	edi
	pop	esi
	ret
;;; --------------------------------------

;;; -------
;; DispAL
;;; -------
DispAL:
	push	ecx,
	push	edx

	mov	ah, 0ch
	mov	dl, al
	shr	al, 4
	mov	ecx, 2
.loop:	
	and	al, 01111b
	cmp	al, 9
	ja	.1
	add	al, '0'
	jmp	.2
.1:
	sub	al, 0Ah
	add	al, 'A'
.2:
	mov	[gs:edi], ax
	add	edi, 2

	mov	al, dl
	loop	.loop
	add	edi, 2

	pop	edx
	pop	ecx

	ret
;;; -----------------------------------------------------

;;; ---------
;; DispReturn
;;; ---------
DispReturn:
	push	eax
	push	ebx
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0ffh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	ebx
	pop	eax

	ret

SegCode32Len	equ	$ - LABEL_SEG_CODE32	
;;; ------------------------------------------

[SECTION .s16code]
ALIGN	32
[BITS 16]
LABEL_SEG_CODE16:
	mov	ax, SELECTOR_NORMAL
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	mov	eax, cr0
	and	al, 11111110b
	mov	cr0, eax

LABEL_BACK_TO_REAL:
	jmp	0:LABEL_REAL_ENTRY
	
	;; SegCode162Len equ $ - LABEL_SEG_CODE16
	   
