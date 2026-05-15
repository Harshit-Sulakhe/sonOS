#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

/* Page directory/table entry flags */
#define PAGE_PRESENT  0x1
#define PAGE_WRITABLE 0x2
#define PAGE_USER     0x4

void paging_init();
void paging_map(uint32_t virt, uint32_t phys, uint32_t flags);

#endif