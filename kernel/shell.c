#include "shell.h"
#include "vfs.h"
#include "syscall.h"
#include "initrd.h"

static char *vga = (char*)0xB8000;

/* Shell display area starts at row 14 */
static int sh_row = 14;
static int sh_col = 0;

/* Input buffer */
static char input[128];
static int  input_len = 0;

static void sh_putchar(char c, uint8_t color) {
    if (c == '\n') {
        sh_row++;
        sh_col = 0;
        if (sh_row >= 23) sh_row = 14;
        return;
    }
    int off = (sh_row * 80 + sh_col) * 2;
    vga[off]   = c;
    vga[off+1] = color;
    sh_col++;
    if (sh_col >= 80) { sh_col = 0; sh_row++; }
}

static void sh_print(const char *msg, uint8_t color) {
    for (int i = 0; msg[i]; i++)
        sh_putchar(msg[i], color);
}

static void sh_print_int(uint32_t n, uint8_t color) {
    if (n == 0) { sh_putchar('0', color); return; }
    char buf[12]; int i = 0;
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    for (int a = 0, b = i-1; a < b; a++, b--) {
        char t = buf[a]; buf[a] = buf[b]; buf[b] = t;
    }
    buf[i] = 0;
    sh_print(buf, color);
}

static void draw_prompt() {
    sh_print("myos> ", 0x0E); /* yellow prompt */
}

/* Compare two strings */
static int sh_strcmp(const char *a, const char *b) {
    int i = 0;
    while (a[i] && b[i] && a[i] == b[i]) i++;
    return a[i] - b[i];
}

/* Check if string starts with prefix */
static int sh_startswith(const char *str, const char *pre) {
    int i = 0;
    while (pre[i]) {
        if (str[i] != pre[i]) return 0;
        i++;
    }
    return 1;
}

static void cmd_help() {
    sh_print("\nAvailable commands:\n", 0x0B);
    sh_print("  help      - show this message\n",   0x0F);
    sh_print("  ls        - list files\n",           0x0F);
    sh_print("  cat <file>- read a file\n",          0x0F);
    sh_print("  echo <msg>- print a message\n",      0x0F);
    sh_print("  clear     - clear shell area\n",     0x0F);
    sh_print("  about     - about this OS\n",        0x0F);
}

static void cmd_ls() {
    sh_print("\nFiles in initrd:\n", 0x0B);
    int found = 0;
    for (int i = 0; i < INITRD_MAX_FILES; i++) {
        initrd_file_t *f = initrd_get(i);
        if (f) {
            sh_print("  ", 0x0F);
            sh_print(f->name, 0x0A);
            sh_print("  (", 0x07);
            sh_print_int(f->size, 0x07);
            sh_print(" bytes)\n", 0x07);
            found++;
        }
    }
    if (!found) sh_print("  (no files)\n", 0x07);
}

static void cmd_cat(const char *filename) {
    vfs_node_t *node = vfs_open(filename);
    if (!node) {
        sh_print("\nFile not found: ", 0x0C);
        sh_print(filename, 0x0C);
        sh_putchar('\n', 0x0F);
        return;
    }
    uint8_t buf[VFS_MAX_DATA];
    uint32_t len = vfs_read(node, buf, node->size);
    sh_putchar('\n', 0x0F);
    for (uint32_t i = 0; i < len; i++)
        sh_putchar(buf[i], 0x0F);
    sh_putchar('\n', 0x0F);
    vfs_close(node);
}

static void cmd_echo(const char *msg) {
    sh_putchar('\n', 0x0F);
    sh_print(msg, 0x0F);
    sh_putchar('\n', 0x0F);
}

static void cmd_clear() {
    for (int r = 14; r < 24; r++) {
        for (int c = 0; c < 80; c++) {
            vga[(r * 80 + c) * 2]     = ' ';
            vga[(r * 80 + c) * 2 + 1] = 0x0F;
        }
    }
    sh_row = 14;
    sh_col = 0;
}

static void cmd_about() {
    sh_print("\n  MyOS v1.0\n", 0x0B);
    sh_print("  Built from scratch in C + NASM\n", 0x0F);
    sh_print("  Running on bare x86 hardware\n",   0x0F);
    sh_print("  Phases complete: 8/8\n",            0x0A);
}

static void execute(const char *cmd) {
    if (sh_strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (sh_strcmp(cmd, "ls") == 0) {
        cmd_ls();
    } else if (sh_strcmp(cmd, "clear") == 0) {
        cmd_clear();
    } else if (sh_strcmp(cmd, "about") == 0) {
        cmd_about();
    } else if (sh_startswith(cmd, "cat ")) {
        cmd_cat(cmd + 4);
    } else if (sh_startswith(cmd, "echo ")) {
        cmd_echo(cmd + 5);
    } else if (cmd[0] != '\0') {
        sh_print("\nUnknown command: ", 0x0C);
        sh_print(cmd, 0x0C);
        sh_print("\nType 'help' for commands\n", 0x07);
    }
}

void shell_init() {
    sh_row = 18;
    sh_col = 0;
    input_len = 0;

    /* Draw divider line between kernel output and shell */
    for (int c = 0; c < 80; c++) {
        vga[(13 * 80 + c) * 2]     = '-';
        vga[(13 * 80 + c) * 2 + 1] = 0x08;
    }

    sh_print("MyOS Shell - type 'help' for commands\n", 0x0B);
    draw_prompt();
}

void shell_handle_key(char c) {
    if (c == '\n' || c == '\r') {
        /* Execute command */
        input[input_len] = '\0';
        sh_putchar('\n', 0x0F);
        execute(input);
        /* Reset input */
        input_len = 0;
        input[0]  = '\0';
        sh_putchar('\n', 0x0F);
        draw_prompt();
    } else if (c == '\b') {
        /* Backspace */
        if (input_len > 0) {
            input_len--;
            if (sh_col > 0) {
                sh_col--;
                int off = (sh_row * 80 + sh_col) * 2;
                vga[off]   = ' ';
                vga[off+1] = 0x0F;
            }
        }
    } else if (input_len < 127) {
        /* Add character to input and display it */
        input[input_len++] = c;
        sh_putchar(c, 0x0F);
    }
}

void shell_run() {
    /* Shell runs via keyboard interrupts */
    while(1) asm volatile("hlt");
}