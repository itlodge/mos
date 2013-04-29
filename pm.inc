;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Descriptor				      ;;
;; usage: Descriptor Base, Limit, Attr	      ;;
;; Base:	dd			      ;;
;; Limit:	dd			      ;;
;; Attr:	dw			      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; 描述符类型
DA_32 EQU 4000h			; 32位段

;; 存储段
DA_C EQU 98h			; 只执行代码段属性值
DA_DRW	EQU 92h			; 可读写数据段属性值
	
%macro Descriptor 3
	dw	%2 & 0FFFFh
	dw	%1 & 0FFFFh
	db	(%1 >> 16) & 0FFh
	dw	((%2 >> 8) & 0F00h) | (%3 & 0F0FFh)
	db	(%1 >> 24) & 0FFh
%endmacro
	
