#!/bin/bash

# if [ ! -f "mos.img" ]; then
#     nasm boot.asm -o boot.bin
#     dd if=/dev/null of=mos.img bs=512 count=2880
#     dd if=boot.bin of=mos.img bs=512 count=1 conv=notrunc
# fi
# nasm pmtest.asm -o pmtest.bin
# dd if=pmtest.bin of=mos.img bs=512 count=1 conv=notrunc

# echo "" | bochs

function simu()
{
    if [ -f "pm.img" ]; then
	rm pm.img
    fi
    bximage -fd -size=1.44 -q pm.img
    echo "abc" | sudo -S losetup /dev/loop0 pm.img
    sudo mkfs.ntfs /dev/loop0
    sudo losetup -d /dev/loop0

    echo "" | bochs&
}

function com()
{
    nasm pmtest.asm -o pmtest.com

    sudo mount -t tmpfs -o loop pm.img /mnt/
    sudo cp pmtest.com /mnt/
    sudo umount /mnt
}

if [ "$1" == "simu" ]; then
    simu
elif [ "$1" == "com" ]; then
    com
fi



