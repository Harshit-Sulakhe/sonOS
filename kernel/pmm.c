#include "pmm.h"

/* Bitmap: 1 bit per page. 1 = used, 0 = free */
static uint32_t bitmap[PMM_MAX_PAGES / 32];
static uint32_t total_pages = 0;
static uint32_t free_pages  = 0;

/* Kernel ends here — defined in linker script */
extern uint32_t kernel_end;

static void bitmap_set(uint32_t bit) {
    bitmap[bit / 32] |= (1 << (bit % 32));
}
static void bitmap_clear(uint32_t bit) {
    bitmap[bit / 32] &= ~(1 << (bit % 32));
}
static int bitmap_test(uint32_t bit) {
    return bitmap[bit / 32] & (1 << (bit % 32));
}

void pmm_init(uint32_t mem_size) {
    total_pages = mem_size / PAGE_SIZE;
    free_pages  = total_pages;

    /* Mark all pages as used initially */
    for (uint32_t i = 0; i < PMM_MAX_PAGES / 32; i++)
        bitmap[i] = 0xFFFFFFFF;

    /* Free pages above the kernel */
    uint32_t kernel_end_page =
        ((uint32_t)&kernel_end / PAGE_SIZE) + 1;

    for (uint32_t i = kernel_end_page; i < total_pages; i++) {
        bitmap_clear(i);
    }
    free_pages = total_pages - kernel_end_page;
}

void *pmm_alloc_page() {
    for (uint32_t i = 0; i < total_pages; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            free_pages--;
            return (void*)(i * PAGE_SIZE);
        }
    }
    return 0; /* out of memory */
}

void pmm_free_page(void *addr) {
    uint32_t page = (uint32_t)addr / PAGE_SIZE;
    bitmap_clear(page);
    free_pages++;
}

uint32_t pmm_get_free_pages() {
    return free_pages;
}