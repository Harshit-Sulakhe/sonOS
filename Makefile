CC = i686-elf-gcc
AS = nasm

CFLAGS = -ffreestanding -O2 -Wall -Ikernel

all: myos.iso

# Bootloader
boot.o: boot/boot.asm
	$(AS) -f elf32 $< -o $@

# Kernel core
kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

gdt.o: kernel/gdt.c
	$(CC) $(CFLAGS) -c $< -o $@

idt.o: kernel/idt.c
	$(CC) $(CFLAGS) -c $< -o $@



# Interrupt C handlers
isr.o: kernel/isr.c
	$(CC) $(CFLAGS) -c $< -o $@

# Memory management
pmm.o: kernel/pmm.c
	$(CC) $(CFLAGS) -c $< -o $@

paging.o: kernel/paging.c
	$(CC) $(CFLAGS) -c $< -o $@

kheap.o: kernel/kheap.c
	$(CC) $(CFLAGS) -c $< -o $@

# Process + scheduler
process.o: kernel/process.c
	$(CC) $(CFLAGS) -c $< -o $@

scheduler.o: kernel/scheduler.c
	$(CC) $(CFLAGS) -c $< -o $@

# Filesystem
vfs.o: kernel/vfs.c
	$(CC) $(CFLAGS) -c $< -o $@

initrd.o: kernel/initrd.c
	$(CC) $(CFLAGS) -c $< -o $@

# Syscalls + shell
syscall.o: kernel/syscall.c
	$(CC) $(CFLAGS) -c $< -o $@

shell.o: kernel/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
myos.bin: boot.o kernel.o gdt.o idt.o isr.o pmm.o paging.o kheap.o process.o scheduler.o vfs.o initrd.o syscall.o shell.o
	$(CC) -T linker.ld -ffreestanding -nostdlib -o $@ $^

# Build ISO
myos.iso: myos.bin
	cp myos.bin iso/boot/
	cp grub.cfg iso/boot/grub/
	grub-mkrescue -o myos.iso iso

# Run in QEMU
run: myos.iso
	qemu-system-i386 -cdrom myos.iso -k en-us

# Clean build files
clean:
	rm -f *.o *.bin *.iso