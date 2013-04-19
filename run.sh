#!/bin/bash

nasm boot.asm -o boot.bin
dd if=/dev/null of=mos.img bs=512 count=2880
dd if=boot.bin of=mos.img bs=512 count=1 conv=notrunc
echo "" | bochs
