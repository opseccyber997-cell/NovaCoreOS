.RECIPEPREFIX := >

CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -Iinclude -c
LDFLAGS = -m elf_i386 -T kernel/linker.ld

OBJECTS = kernel/boot.o \
          kernel/interrupts_asm.o \
          kernel/kernel.o \
          kernel/print.o \
          kernel/keyboard.o \
          kernel/shell.o \
          kernel/interrupts.o \
          kernel/exceptions.o \
          kernel/timer.o \
          kernel/memory.o

all: NovaCoreOS.iso

kernel/boot.o: kernel/boot.asm
>$(AS) -f elf32 kernel/boot.asm -o kernel/boot.o

kernel/interrupts_asm.o: kernel/interrupts.asm
>$(AS) -f elf32 kernel/interrupts.asm -o kernel/interrupts_asm.o

kernel/kernel.o: kernel/kernel.c include/print.h include/shell.h include/interrupts.h include/timer.h include/memory.h
>$(CC) $(CFLAGS) kernel/kernel.c -o kernel/kernel.o

kernel/print.o: kernel/print.c include/print.h include/io.h
>$(CC) $(CFLAGS) kernel/print.c -o kernel/print.o

kernel/keyboard.o: kernel/keyboard.c include/keyboard.h include/shell.h include/print.h include/io.h
>$(CC) $(CFLAGS) kernel/keyboard.c -o kernel/keyboard.o

kernel/shell.o: kernel/shell.c include/shell.h include/print.h include/io.h include/timer.h include/memory.h
>$(CC) $(CFLAGS) kernel/shell.c -o kernel/shell.o

kernel/interrupts.o: kernel/interrupts.c include/interrupts.h include/io.h
>$(CC) $(CFLAGS) kernel/interrupts.c -o kernel/interrupts.o

kernel/exceptions.o: kernel/exceptions.c include/exceptions.h include/interrupts.h include/print.h
>$(CC) $(CFLAGS) kernel/exceptions.c -o kernel/exceptions.o

kernel/timer.o: kernel/timer.c include/timer.h include/io.h
>$(CC) $(CFLAGS) kernel/timer.c -o kernel/timer.o

kernel/memory.o: kernel/memory.c include/memory.h include/print.h
>$(CC) $(CFLAGS) kernel/memory.c -o kernel/memory.o

kernel.bin: $(OBJECTS)
>$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.bin

NovaCoreOS.iso: kernel.bin boot/grub/grub.cfg
>mkdir -p iso/boot/grub
>cp kernel.bin iso/boot/kernel.bin
>cp boot/grub/grub.cfg iso/boot/grub/grub.cfg
>grub-mkrescue -o NovaCoreOS.iso iso

run: NovaCoreOS.iso
>qemu-system-i386 -cdrom NovaCoreOS.iso

clean:
>rm -f kernel/*.o kernel.bin NovaCoreOS.iso
>rm -rf iso/boot

.PHONY: all run clean
