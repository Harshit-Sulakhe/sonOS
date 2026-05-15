#include "idt.h"
#include "isr.h"
#include <stdint.h>

static struct idt_entry idt[256];
static struct idt_ptr   ip;

extern void idt_flush(uint32_t);

static void idt_set_gate(uint8_t num, uint32_t base,
                         uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = sel;
    idt[num].zero      = 0;
    idt[num].flags = flags;
}

void idt_init() {
    ip.limit = (sizeof(struct idt_entry) * 256) - 1;
    ip.base  = (uint32_t)&idt;

    /* CPU exceptions 0-31 */
    idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);
    idt_set_gate(1,  (uint32_t)isr1,  0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);

    /* Hardware IRQs 32-47 */
    idt_set_gate(32, (uint32_t)irq0,  0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1,  0x08, 0x8E);

    idt_flush((uint32_t)&ip);
}