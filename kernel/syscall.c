#include "syscall.h"
#include "vfs.h"
#include "idt.h"

static char *vga = (char*)0xB8000;
static int syscall_col = 0;
static int syscall_row = 14;

static void vga_putchar(char c, uint8_t color) {
    if (c == '\n') {
        syscall_row++;
        syscall_col = 0;
        return;
    }
    if (syscall_row >= 23) {
        /* Scroll up — shift all rows up by one */
        for (int r = 14; r < 23; r++) {
            for (int c2 = 0; c2 < 80; c2++) {
                vga[(r * 80 + c2) * 2]     =
                    vga[((r+1) * 80 + c2) * 2];
                vga[(r * 80 + c2) * 2 + 1] =
                    vga[((r+1) * 80 + c2) * 2 + 1];
            }
        }
        /* Clear last row */
        for (int c2 = 0; c2 < 80; c2++) {
            vga[(23 * 80 + c2) * 2]     = ' ';
            vga[(23 * 80 + c2) * 2 + 1] = 0x0F;
        }
        syscall_row = 23;
        syscall_col = 0;
    }
    int off = (syscall_row * 80 + syscall_col) * 2;
    vga[off]   = c;
    vga[off+1] = color;
    syscall_col++;
    if (syscall_col >= 80) {
        syscall_col = 0;
        syscall_row++;
    }
}

void syscall_init() {
    syscall_col = 0;
    syscall_row = 14;
}

uint32_t syscall_handler(uint32_t num,
                         uint32_t arg1,
                         uint32_t arg2,
                         uint32_t arg3) {
    switch (num) {
        case SYS_WRITE: {
            /* arg1 = fd (1=stdout), arg2 = buf, arg3 = len */
            if (arg1 == 1) {
                char *buf = (char*)arg2;
                for (uint32_t i = 0; i < arg3; i++)
                    vga_putchar(buf[i], 0x0F);
            }
            return arg3;
        }
        case SYS_READ: {
            /* Simple: read from initrd file */
            vfs_node_t *node = (vfs_node_t*)arg1;
            if (!node) return 0;
            return vfs_read(node, (uint8_t*)arg2, arg3);
        }
        case SYS_OPEN: {
            /* arg1 = filename string pointer */
            vfs_node_t *node = vfs_open((const char*)arg1);
            return (uint32_t)node;
        }
        case SYS_CLOSE: {
            vfs_close((vfs_node_t*)arg1);
            return 0;
        }
        case SYS_EXIT: {
            /* Just halt for now */
            asm volatile("cli; hlt");
            return 0;
        }
    }
    return 0;
}