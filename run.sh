#!/bin/bash

# if [ ! -f "mos.img" ]; then
#     nasm boot.asm -o boot.bin
#     dd if=/dev/null of=mos.img bs=512 count=2880
#     dd if=boot.bin of=mos.img bs=512 count=1 conv=notrunc
# fi
# nasm pmtest.asm -o pmtest.bin
# dd if=pmtest.bin of=mos.img bs=512 count=1 conv=notrunc

# echo "" | bochs

if [ -f "pm.img" ]; then
    rm pm.img
fi
bximage -fd -size=1.44 -q pm.img
echo "" | bochs&

nasm pmtest.asm -o pmtest.com

sudo mount -o loop pm.img /mnt/
sudo rm -r /mnt/*
sudo cp pmtest.com /mnt/
sudo umount /mnt


