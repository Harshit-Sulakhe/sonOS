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
#include "ata.h"

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

    /* ATA disk driver */
if (ata_init() == 0) {
    print("[OK] ATA disk found\n", 0x0A);

    /* Test: write a message to sector 100 */
    uint8_t wbuf[512];
    for (int i = 0; i < 512; i++) wbuf[i] = 0;
    const char *msg = "Hello from MyOS disk!";
    for (int i = 0; msg[i]; i++) wbuf[i] = msg[i];

    if (ata_write(100, wbuf, 1) == 0) {
        print("[OK] ATA write sector 100\n", 0x0A);
    }

    /* Test: read it back */
    uint8_t rbuf[512];
    if (ata_read(100, rbuf, 1) == 0) {
        print("[OK] ATA read sector 100: ", 0x0A);
        /* Print first 21 chars of what we read */
        for (int i = 0; i < 21; i++) {
            if (rbuf[i]) {
                int off = (row * 80 + col) * 2;
                vga[off]   = rbuf[i];
                vga[off+1] = 0x0E;
                col++;
            }
        }
        print("\n", 0x0F);
    }
} else {
    print("[--] No ATA disk (add -hda to QEMU)\n", 0x0E);
}

    /* Phase 7 */
   /* process_init();
    scheduler_init();
    print("[OK] Scheduler\n", 0x0A);
    process_create("Task-A", task_a);
    process_create("Task-B", task_b);
    print("[OK] Processes created\n", 0x0A); */

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

    asm volatile("sti");
    print("[OK] Interrupts\n", 0x0A);

    print("[OK] Step 1\n", 0x0A);
    
    /* Draw divider manually instead of calling shell_init */
    for (int i = 0; i < 80; i++) {
        vga[(17 * 80 + i) * 2]     = '-';
        vga[(17 * 80 + i) * 2 + 1] = 0x08;
    }
    print("[OK] Step 2\n", 0x0A);

    /* Test shell directly */
    vga[(18 * 80 + 0) * 2]     = 'm';
    vga[(18 * 80 + 0) * 2 + 1] = 0x0F;
    vga[(18 * 80 + 1) * 2]     = 'y';
    vga[(18 * 80 + 1) * 2 + 1] = 0x0F;
    vga[(18 * 80 + 2) * 2]     = 'o';
    vga[(18 * 80 + 2) * 2 + 1] = 0x0F;
    vga[(18 * 80 + 3) * 2]     = 's';
    vga[(18 * 80 + 3) * 2 + 1] = 0x0F;
    vga[(18 * 80 + 4) * 2]     = '>';
    vga[(18 * 80 + 4) * 2 + 1] = 0x0E;
    print("[OK] Step 3\n", 0x0A);

    shell_init();
    print("[OK] Shell init done\n", 0x0A);

    while(1) asm volatile("hlt");
}