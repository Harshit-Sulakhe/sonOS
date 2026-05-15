#include "isr.h"
#include "io.h"
#include "scheduler.h"
#include "shell.h"

static char *vga = (char*)0xB8000;
static int cursor = 160;

static void print(const char *msg) {
    for (int i = 0; msg[i]; i++) {
        vga[cursor++] = msg[i];
        vga[cursor++] = 0x0F;
    }
}

/* Scancode to ASCII translation table */
static const char keymap[] = {
    0,   0,  '1','2','3','4','5','6',
  '7', '8', '9','0','-','=', 0,  0,
  'q', 'w', 'e','r','t','y','u','i',
  'o', 'p', '[',']','\n', 0, 'a','s',
  'd', 'f', 'g','h','j','k','l',';',
  '\'','`',  0, '\\','z','x','c','v',
  'b', 'n', 'm',',','.','/', 0,  0,
   0,  ' '
};

void isr_handler(uint32_t num) {
    if (num == 0)  print("Exception: Divide by zero! ");
    if (num == 14) print("Exception: Page fault!     ");
}

void irq_handler(uint32_t num) {
    if (num == 0) {
        scheduler_tick();
    }

    if (num == 1) {
        uint8_t scancode = inb(0x60);

        if (!(scancode & 0x80)) {
            if (scancode < sizeof(keymap)) {
                char c = keymap[scancode];
                if (c) shell_handle_key(c);
            }
        }
    }

    if (num >= 8) outb(0xA0, 0x20);
    outb(0x20, 0x20);
}