#include "gdt.h"
#include "idt.h"
#include "io.h"
#include "pmm.h"
#include "paging.h"
#include "kheap.h"
#include "ata.h"
#include "vfs.h"
#include "initrd.h"
#include "syscall.h"
#include "shell.h"

static char *vga = (char*)0xB8000;
static int col = 0, row = 0;

static void print(const char *msg, uint8_t color) {
    for (int i = 0; msg[i]; i++) {
        if (msg[i] == '\n') { row++; col = 0; continue; }
        int off = (row * 80 + col) * 2;
        vga[off]   = msg[i];
        vga[off+1] = color;
        col++;
    }
}

void kernel_main() {
    /* Clear screen */
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vga[i] = ' '; vga[i+1] = 0x0F;
    }

    print("MyOS Booting...\n", 0x0F);

    gdt_init();
    print("[OK] GDT\n", 0x0A);

    idt_init();
    print("[OK] IDT\n", 0x0A);

    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0xFD);outb(0xA1, 0xFF);
    print("[OK] PIC\n", 0x0A);

    pmm_init(128 * 1024 * 1024);
    print("[OK] PMM\n", 0x0A);

    paging_init();
    print("[OK] Paging\n", 0x0A);

    kheap_init();
    print("[OK] Heap\n", 0x0A);

    /* ATA */
    if (ata_init() == 0) {
        print("[OK] ATA disk found\n", 0x0A);
        uint8_t wbuf[512];
        for (int i = 0; i < 512; i++) wbuf[i] = 0;
        const char *msg = "Hello from MyOS disk!";
        for (int i = 0; msg[i]; i++) wbuf[i] = msg[i];
        if (ata_write(100, wbuf, 1) == 0)
            print("[OK] ATA write\n", 0x0A);
        uint8_t rbuf[512];
        if (ata_read(100, rbuf, 1) == 0)
            print("[OK] ATA read\n", 0x0A);
    } else {
        print("[--] No ATA disk\n", 0x0E);
    }

    vfs_init();
    initrd_init();
    initrd_create("readme.txt",
        "Welcome to MyOS!\n"
        "Type 'help' for commands.\n", 44);
    initrd_create("hello.txt",
        "Hello from MyOS filesystem!\n", 28);
    initrd_create("info.txt",
        "MyOS v1.0\nKernel: x86 32-bit\n", 30);
    print("[OK] VFS + initrd\n", 0x0A);
        
    syscall_init();
    print("[OK] Syscalls\n", 0x0A);
       
    /* Enable interrupts */
    asm volatile("sti");
    print("[OK] Interrupts\n", 0x0A);
        
    /* Launch shell */
    shell_init();
    print("[OK] Shell init\n", 0x0A);

    while(1) asm volatile("hlt");
}