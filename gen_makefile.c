#include <stdio.h>

int main() {
    FILE *f = fopen("Makefile", "w");
    if (!f) { printf("Failed!\n"); return 1; }

    fprintf(f, "CC = i686-elf-gcc\n");
    fprintf(f, "AS = nasm\n");
    fprintf(f, "CFLAGS = -ffreestanding -O2 -Wall -Ikernel\n");
    fprintf(f, "\n");
    fprintf(f, "all: myos.iso\n");
    fprintf(f, "\n");
    fprintf(f, "boot.o: boot/boot.asm\n");
    fprintf(f, "\t$(AS) -f elf32 $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "kernel.o: kernel/kernel.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "gdt.o: kernel/gdt.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "idt.o: kernel/idt.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "isr.o: kernel/isr.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "pmm.o: kernel/pmm.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "paging.o: kernel/paging.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "kheap.o: kernel/kheap.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "process.o: kernel/process.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "scheduler.o: kernel/scheduler.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "vfs.o: kernel/vfs.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "initrd.o: kernel/initrd.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "syscall.o: kernel/syscall.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "shell.o: kernel/shell.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "ata.o: kernel/ata.c\n");
    fprintf(f, "\t$(CC) $(CFLAGS) -c $< -o $@\n");
    fprintf(f, "\n");
    fprintf(f, "myos.bin: boot.o kernel.o gdt.o idt.o isr.o pmm.o paging.o kheap.o process.o scheduler.o vfs.o initrd.o syscall.o shell.o ata.o\n");
    fprintf(f, "\t$(CC) -T linker.ld -ffreestanding -nostdlib -o $@ $^\n");
    fprintf(f, "\n");
    fprintf(f, "myos.iso: myos.bin\n");
    fprintf(f, "\tcp myos.bin iso/boot/\n");
    fprintf(f, "\tcp grub.cfg iso/boot/grub/\n");
    fprintf(f, "\tgrub-mkrescue -o myos.iso iso\n");
    fprintf(f, "\n");
    fprintf(f, "run: myos.iso\n");
    fprintf(f, "\tqemu-system-i386 -cdrom myos.iso -drive file=disk.img,format=raw,index=0,media=disk -k en-us -boot d\n");
    fprintf(f, "\n");
    fprintf(f, "clean:\n");
    fprintf(f, "\trm -f *.o *.bin *.iso\n");

    fclose(f);
    printf("Makefile generated successfully!\n");
    return 0;
}