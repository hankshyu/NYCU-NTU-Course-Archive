#!/bin/bash

make	
yasm -f elf64 -DYASM -D__x86_64__ -DPIC start.asm -o start.o
gcc -c -g -Wall -fno-stack-protector -nostdlib -I. -I.. -DUSEMINI $1.c
ld -m elf_x86_64 --dynamic-linker /lib64/ld-linux-x86-64.so.2 -o $1 $1.o start.o -L. -L.. -lmini
rm $1.o
echo "--------------------"
LD_LIBRARY_PATH=. ./$1
echo "--------------------"
echo $?
echo "--------------------"
make clean
rm $1
