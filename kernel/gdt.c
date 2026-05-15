#include "gdt.h"

static struct gdt_entry gdt[3];
static struct gdt_ptr   gp;


extern void gdt_flush(uint32_t);

static void gdt_set_entry(int i, uint32_t base, uint32_t limit,
                          uint8_t access, uint8_t gran) {
    gdt[i].base_low    = (base & 0xFFFF);
    gdt[i].base_middle = (base >> 16) & 0xFF;
    gdt[i].base_high   = (base >> 24) & 0xFF;
    gdt[i].limit_low   = (limit & 0xFFFF);
    gdt[i].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[i].access      = access;
}

void gdt_init() {
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base  = (uint32_t)&gdt;

    gdt_set_entry(0, 0, 0,          0,    0);    /* null */
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* code */
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* data */

    gdt_flush((uint32_t)&gp);
}