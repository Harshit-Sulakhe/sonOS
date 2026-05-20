#include "syscall.h"
#include "vfs.h"

static char *vga = (char*)0xB8000;
static int syscall_col = 0;
static int syscall_row = 19;

static void vga_putchar(char c, uint8_t color) {
    if (c == '\n') {
        syscall_row++;
        syscall_col = 0;
        if (syscall_row >= 23) syscall_row = 19;
        return;
    }
    int off = (syscall_row * 80 + syscall_col) * 2;
    vga[off]   = c;
    vga[off+1] = color;
    syscall_col++;
    if (syscall_col >= 80) {
        syscall_col = 0;
        syscall_row++;
        if (syscall_row >= 23) syscall_row = 19;
    }
}

void syscall_init() {
    syscall_col = 0;
    syscall_row = 19;
}

uint32_t syscall_handler(uint32_t num,
                         uint32_t arg1,
                         uint32_t arg2,
                         uint32_t arg3) {
    switch (num) {
        case SYS_WRITE: {
            if (arg1 == 1) {
                char *buf = (char*)arg2;
                for (uint32_t i = 0; i < arg3; i++)
                    vga_putchar(buf[i], 0x0F);
            }
            return arg3;
        }
        case SYS_READ: {
            vfs_node_t *node = (vfs_node_t*)arg1;
            if (!node) return 0;
            return vfs_read(node, (uint8_t*)arg2, arg3);
        }
        case SYS_OPEN: {
            vfs_node_t *node = vfs_open((const char*)arg1);
            return (uint32_t)node;
        }
        case SYS_CLOSE: {
            vfs_close((vfs_node_t*)arg1);
            return 0;
        }
        case SYS_EXIT: {
            asm volatile("cli; hlt");
            return 0;
        }
    }
    return 0;
}