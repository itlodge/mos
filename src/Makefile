#-------------------
# Makefile for MOS
#-------------------

ENTRY_POINT = 0x30400

# Paths
BOOT_PATH = boot
KERNEL_PATH = kernel
LIB_PATH = lib

# 
ASM = nasm
CC = gcc
LD = ld
ASM_DOS_FLAGS = -I $(BOOT_PATH)/include/
ASM_ELF_FLAGS = -I $(BOOT_PATH)/include/ -f elf
C_FLAGS = -I include -c -fno-builtin -m32 -std=c99
LD_FLAGS = -s -m elf_i386 -Ttext $(ENTRY_POINT)

BOOT_SRC = $(BOOT_PATH)/boot.asm

LOADER_SRC = $(BOOT_PATH)/loader.asm 

KERNEL_SRC = $(KERNEL_PATH)/kernel.asm $(LIB_PATH)/klib.asm 
KERNEL_C = $(KERNEL_PATH)/start.c $(KERNEL_PATH)/i8259.c \
	$(KERNEL_PATH)/protect.c $(LIB_PATH)/klibc.c

KERNEL_OBJS = $(KERNEL_SRC:.asm=.o)
KERNEL_CO = $(KERNEL_C:.c=.o)

BOOT_BIN = $(BOOT_SRC:.asm=.bin)
LOADER_BIN = $(LOADER_SRC:.asm=.bin)

KERNEL_BIN = $(KERNEL_PATH)/kernel.bin

IMG = a.img
FLOPPY = /mnt

.PHONY : all buildimage clean

all: buildimage
	qemu-system-i386 -fda a.img

buildimage: $(BOOT_BIN) $(LOADER_BIN) $(KERNEL_BIN)
	dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp $(LOADER_BIN) $(KERNEL_BIN) $(FLOPPY)
	sudo umount $(FLOPPY)

$(BOOT_BIN): $(BOOT_SRC)
	$(ASM) $(ASM_DOS_FLAGS) -o $@ $<

$(LOADER_BIN): $(LOADER_SRC)
	$(ASM) $(ASM_DOS_FLAGS) -o $@ $< 

$(KERNEL_OBJS): $(KERNEL_SRC)
	$(ASM) $(ASM_ELF_FLAGS) -o $@ $(@:.o=.asm)

$(KERNEL_CO): $(KERNEL_C)
	$(CC) $(C_FLAGS) -o $@ $(@:.o=.c)

$(KERNEL_BIN): $(KERNEL_OBJS) $(KERNEL_CO)
	$(LD) $(LD_FLAGS) $(KERNEL_OBJS) $(KERNEL_CO) -o $@

clean :
	$(RM) $(BOOT_BIN) $(LOADER_BIN) $(KERNEL_BIN) $(KERNEL_OBJS) \
	$(KERNEL_CO)

