;; FAT12
;; ======================================================================
BS_OEMName       db      'Yuanhang'; OEM name, must be 8 bytes
BPB_BytsPerSec   dw      512      ; Bytes per sector
BPB_SecPerClus   db      1        ; Sectors per cluster
BPB_RsvdSecCnt   dw      1        ; Sectors occupied by Boot record
BPB_NumFATs      db      2        ; Number of FAT table
BPB_RootEntCnt   dw      224      ; Max number of root entry
BPB_TotSec16     dw      2880     ; Total numbers of logical sector
BPB_Media        db      0xF0     ; Media descriptor
BPB_FATSz16      dw      9        ; Sectors per FAT
BPB_SecPerTrk    dw      18       ; Sectors per track
BPB_NumHeads     dw      2        ; Number of head
BPB_HiddSec      dd      0        ; Number of hidden sector
BPB_TotSec32     dd      0        ; Sectors if BPB_TotSec16 is zero
BS_DrvNum        db      0        ; Driver id for interrupt 13
BS_Reserved1     db      0        ; Not used
BS_BootSig       db      29h      ;
BS_VolID         dd      0        ; Volumn id
BS_VolLab        db      'MOS0.0.1   '    ; Volumn label, 11 bytes
BS_FileSysType   db      'FAT12   ' ; Type of file system, 8 bytes
;; =======================================================================

;; Constant for FAT12
ROOT_DIR_SECTORS        equ     14
SECTOR_NUM_OF_ROOT_DIR  equ     19
SECTOR_NUM_OF_FAT1      equ     1
DELTA_SECTOR_NUM        equ     17
