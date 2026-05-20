CC = i686-elf-gcc
AS = nasm
CFLAGS = -ffreestanding -O2 -Wall -Ikernel

all: myos.iso

boot.o: boot/boot.asm
	$(AS) -f elf32 $< -o $@

kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

gdt.o: kernel/gdt.c
	$(CC) $(CFLAGS) -c $< -o $@

idt.o: kernel/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

isr.o: kernel/isr.c
	$(CC) $(CFLAGS) -c $< -o $@

pmm.o: kernel/pmm.c
	$(CC) $(CFLAGS) -c $< -o $@

paging.o: kernel/paging.c
	$(CC) $(CFLAGS) -c $< -o $@

kheap.o: kernel/kheap.c
	$(CC) $(CFLAGS) -c $< -o $@

process.o: kernel/process.c
	$(CC) $(CFLAGS) -c $< -o $@

scheduler.o: kernel/scheduler.c
	$(CC) $(CFLAGS) -c $< -o $@

vfs.o: kernel/vfs.c
	$(CC) $(CFLAGS) -c $< -o $@

initrd.o: kernel/initrd.c
	$(CC) $(CFLAGS) -c $< -o $@

syscall.o: kernel/syscall.c
	$(CC) $(CFLAGS) -c $< -o $@

shell.o: kernel/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

ata.o: kernel/ata.c
	$(CC) $(CFLAGS) -c $< -o $@

myos.bin: boot.o kernel.o gdt.o idt.o isr.o pmm.o paging.o kheap.o process.o scheduler.o vfs.o initrd.o syscall.o shell.o ata.o
	$(CC) -T linker.ld -ffreestanding -nostdlib -o $@ $^

myos.iso: myos.bin
	cp myos.bin iso/boot/
	cp grub.cfg iso/boot/grub/
	grub-mkrescue -o myos.iso iso

run: myos.iso
	qemu-system-i386 -cdrom myos.iso -drive file=disk.img,format=raw -k en-us -no-reboot -no-shutdown

clean:
	rm -f *.o *.bin *.iso
