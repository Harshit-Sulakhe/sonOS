#include "gdt.h"
#include "idt.h"
#include "io.h"
#include "pmm.h"
#include "paging.h"
#include "kheap.h"
#include "process.h"
#include "scheduler.h"
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

    /* Phase 5 */
    gdt_init();
    print("[OK] GDT\n", 0x0A);
    idt_init();
    print("[OK] IDT\n", 0x0A);

    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0x00); outb(0xA1, 0x00);
    print("[OK] PIC\n", 0x0A);

    /* Phase 6 */
    pmm_init(128 * 1024 * 1024);
    print("[OK] PMM\n", 0x0A);
    paging_init();
    print("[OK] Paging\n", 0x0A);
    kheap_init();
    print("[OK] Heap\n", 0x0A);

    /* Phase 7 */
    process_init();
    scheduler_init();
    print("[OK] Scheduler\n", 0x0A);

    /* Phase 8: VFS + initrd */
    vfs_init();
    initrd_init();

    /* Create some files in the initrd */
    initrd_create("readme.txt",
        "Welcome to MyOS!\n"
        "This OS was built from scratch in C.\n"
        "Type 'help' for available commands.\n",
        80);

    initrd_create("hello.txt",
        "Hello from the MyOS filesystem!\n"
        "You can read and write files.\n",
        63);

    initrd_create("info.txt",
        "MyOS v1.0\n"
        "Kernel: x86 32-bit\n"
        "Memory: 128MB\n"
        "FS: initrd\n",
        52);

    print("[OK] VFS + initrd\n", 0x0A);

    /* Syscall interface */
    syscall_init();
    print("[OK] Syscalls\n", 0x0A);

    /* Enable interrupts */
    asm volatile("sti");
    print("[OK] Interrupts\n", 0x0A);

    /* Launch shell */
    print("[OK] Launching shell...\n", 0x0A);
    shell_init();

    /* Idle loop — everything runs via interrupts */
    while(1) asm volatile("hlt");
}