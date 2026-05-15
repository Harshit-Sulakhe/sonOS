#include "paging.h"
#include "pmm.h"

/* Page directory: 1024 entries × 4 bytes = 4KB */
static uint32_t page_directory[1024] __attribute__((aligned(4096)));

/* One page table covering first 4MB */
static uint32_t page_table_0[1024] __attribute__((aligned(4096)));

void paging_init() {
    /* Clear page directory — mark all as not present */
    for (int i = 0; i < 1024; i++)
        page_directory[i] = 0x00000002; /* not present, writable */

    /* Identity map first 4MB (0x0 → 0x0, 0x1000 → 0x1000 ...) */
    for (int i = 0; i < 1024; i++) {
        page_table_0[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    /* Put page table into directory entry 0 */
    page_directory[0] = ((uint32_t)page_table_0) | PAGE_PRESENT | PAGE_WRITABLE;

    /* Load page directory into CR3 */
    asm volatile("mov %0, %%cr3" :: "r"(page_directory));

    /* Enable paging by setting bit 31 of CR0 */
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

void paging_map(uint32_t virt, uint32_t phys, uint32_t flags) {
    uint32_t dir_idx   = virt >> 22;         /* top 10 bits    */
    uint32_t table_idx = (virt >> 12) & 0x3FF; /* middle 10 bits */

    /* Get or create page table */
    uint32_t *table;
    if (page_directory[dir_idx] & PAGE_PRESENT) {
        table = (uint32_t*)(page_directory[dir_idx] & ~0xFFF);
    } else {
        table = (uint32_t*)pmm_alloc_page();
        for (int i = 0; i < 1024; i++) table[i] = 0;
        page_directory[dir_idx] = (uint32_t)table | PAGE_PRESENT | PAGE_WRITABLE;
    }

    table[table_idx] = phys | flags | PAGE_PRESENT;

    /* Flush TLB for this address */
    asm volatile("invlpg (%0)" :: "r"(virt) : "memory");
}