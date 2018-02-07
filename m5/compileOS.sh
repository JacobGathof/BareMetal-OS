#!/bin/bash

nasm bootload.asm
dd if=/dev/zero of=floppya.img bs=512 count=2880
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc
dd if=wizard.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc
dd if=ghosty.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc

bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o

bcc -ansi -c -o shell.o shell.c
as86 lib.asm -o lib.o
ld86 -o shell -d shell.o lib.o

bcc -ansi -c -o sample.o sample.c
as86 lib.asm -o lib.o
ld86 -o sample -d sample.o lib.o

dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

gcc -o loadFile loadFile.c

./loadFile tstprg
./loadFile message.txt
./loadFile tstpr2
./loadFile shell
./loadFile phello
./loadFile wizard.txt
./loadFile ghosty.txt
./loadFile sample

qemu-system-i386 -fda floppya.img -device isa-debug-exit,iobase=0xf4,iosize=0x04 --boot order=a &
