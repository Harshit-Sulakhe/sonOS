#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define PAGE_SIZE 4096          /* 4KB per page frame    */
#define PMM_MAX_PAGES 32768     /* support up to 128MB   */

void pmm_init(uint32_t mem_size);
void *pmm_alloc_page();
void  pmm_free_page(void *addr);
uint32_t pmm_get_free_pages();

#endif