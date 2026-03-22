ASM=nasm
CC=gcc
LD=ld
OBJCOPY=objcopy

CFLAGS=-ffreestanding -m32 -nostdlib -fno-pic -fno-stack-protector -g -O0 -fno-omit-frame-pointer
LDFLAGS=-m elf_i386 -T linker.ld

OBJS=kernel.o idt.o isr.o vga.o pci.o console.o ata.o

all: os-image

boot.bin: boot.asm
	$(ASM) -f bin boot.asm -o boot.bin

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

idt.o: idt.c
	$(CC) $(CFLAGS) -c idt.c -o idt.o

console.o: console.c
	$(CC) $(CFLAGS) -c console.c -o console.o

vga.o: vga.c
	$(CC) $(CFLAGS) -c vga.c -o vga.o

ata.o: ata.c
	$(CC) $(CFLAGS) -c ata.c -o ata.o

isr.o: isr.asm
	$(ASM) -f elf32 isr.asm -o isr.o

pci.o: pci.c
	$(CC) $(CFLAGS) -c pci.c -o pci.o

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o kernel.elf

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin

os-image: boot.bin kernel.bin
	cat boot.bin kernel.bin > os-image.bin

run: os-image
	qemu-system-i386 -drive format=raw,file=os-image.bin

debug: os-image
	qemu-system-i386 -s -S -drive format=raw,file=os-image.bin

clean:
	rm -f *.bin *.o *.elf