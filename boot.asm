;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; MOS, My first operating system ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	org 7c00h
	mov ax, cs
	mov ds, ax
	mov es, ax
	call display
	jmp $
display:
	mov	ax, BOOT_MESSAGE
	mov	bp, ax
	mov 	cx, [LENGTH]
	mov 	ax, 1301h
	mov 	bx, 000ch
	mov 	dl, 0
	int 	10h
	ret
	
BOOT_MESSAGE:	db	"MOS, my first operating system!"
LENGTH:		db	32

times	510 - ($ - $$)	db	0
dw	0xaa55		
	
