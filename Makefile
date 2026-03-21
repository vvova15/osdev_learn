ASM=nasm
CC=gcc
LD=ld

CFLAGS=-ffreestanding -m32 -nostdlib -fno-pic -fno-stack-protector
LDFLAGS=-m elf_i386 -T linker.ld

all: os-image

boot.bin: boot.asm
	$(ASM) -f bin boot.asm -o boot.bin

kernel.bin: kernel.c idt.c vga.c isr.asm
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o
	$(CC) $(CFLAGS) -c idt.c -o idt.o
	$(CC) $(CFLAGS) -c vga.c -o vga.o
	$(ASM) -f elf32 isr.asm -o isr.o
	$(LD) $(LDFLAGS) kernel.o idt.o isr.o vga.o -o kernel.bin --oformat binary

os-image: boot.bin kernel.bin
	cat boot.bin kernel.bin > os-image.bin

run: os-image
	qemu-system-i386 -drive format=raw,file=os-image.bin

clean:
	rm -f *.bin *.o