
;; ---------------
;; Descriptor Attribute
;; D: Data
;; C: Code
;; S: System
;; R: Read only
;; RW: Read write
;; A: Accessed

DA_32           equ     4000h
DA_LIMIT_4K     equ     8000h
        
;; Descriptor privilege level
DA_DPL0         equ     00h
DA_DPL1         equ     20h
DA_DPL2         equ     40h
DA_DPL3         equ     60h

;; 
DA_DR           equ     90h
DA_DRW          equ     92h
DA_DRWA         equ     93h
DA_C            equ     98h
DA_CR           equ     9Ah
DA_CCO          equ     9Ch
DA_CCOR         equ     9Eh

DA_LDT          equ     82h
DA_TaskGate     equ     85h
DA_386TSS       equ     89h
DA_386CGate     equ     8Ch     ; Call gate
DA_386IGate     equ     8Eh     ; Interrupt gate
DA_386TGate     equ     8Fh     ; Trap gate

;; Selector attributes
SA_RPL0 equ     0
SA_RPL1 equ     1
SA_RPL2 equ     2
SA_RPL3 equ     3

SA_TIG  equ     0       
SA_TIL  equ     4

;; Paging
PG_P    equ     1               ; Present
PG_RWR  equ     0
PG_RWW  equ     2
PG_USS  equ     0               ; System
PG_USU  equ     4               ; User
        
;; A descriptor need 8 bytes totally
%macro Descriptor 3          ; Need 3 parameters
dw      %2 & 0FFFFh              
dw      %1 & 0FFFFh
db      (%1 >> 16) & 0FFh
dw      ((%2 >> 8) & 0F00h) | (%3 & 0F0FFh)
db      (%1 >> 24) & 0FFh
%endmacro 

