#include "tasks.h"

static char *vga = (char*)0xB8000;

/* Task A: writes A's across row 10 */
void task_a() {
    int col = 0;
    while (1) {
        vga[(10 * 80 + col) * 2]     = 'A';
        vga[(10 * 80 + col) * 2 + 1] = 0x0C; /* red */
        col = (col + 1) % 80;
        for (volatile int i = 0; i < 500000; i++);
    }
}

/* Task B: writes B's across row 11 */
void task_b() {
    int col = 0;
    while (1) {
        vga[(11 * 80 + col) * 2]     = 'B';
        vga[(11 * 80 + col) * 2 + 1] = 0x09; /* blue */
        col = (col + 1) % 80;
        for (volatile int i = 0; i < 500000; i++);
    }
}