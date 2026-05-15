#include "kheap.h"
#include "pmm.h"

/* Each allocation has a small header */
typedef struct block {
    uint32_t      size;   /* size of usable data */
    uint8_t       free;   /* 1 = free, 0 = used  */
    struct block *next;   /* next block in list   */
} block_t;

#define HEAP_SIZE   (1024 * 1024)   /* 1MB heap */
#define HEAP_START  0x200000        /* starts at 2MB mark */

static block_t *heap_start = 0;

void kheap_init() {
    heap_start = (block_t*)HEAP_START;
    heap_start->size = HEAP_SIZE - sizeof(block_t);
    heap_start->free = 1;
    heap_start->next = 0;
}

void *kmalloc(uint32_t size) {
    block_t *cur = heap_start;

    /* Find first free block big enough */
    while (cur) {
        if (cur->free && cur->size >= size) {
            /* Split block if it's much larger */
            if (cur->size > size + sizeof(block_t) + 16) {
                block_t *new_block = (block_t*)
                    ((uint8_t*)cur + sizeof(block_t) + size);
                new_block->size = cur->size - size - sizeof(block_t);
                new_block->free = 1;
                new_block->next = cur->next;
                cur->size = size;
                cur->next = new_block;
            }
            cur->free = 0;
            return (void*)((uint8_t*)cur + sizeof(block_t));
        }
        cur = cur->next;
    }
    return 0; /* out of heap memory */
}

void kfree(void *ptr) {
    if (!ptr) return;

    block_t *block = (block_t*)((uint8_t*)ptr - sizeof(block_t));
    block->free = 1;

    /* Coalesce adjacent free blocks */
    block_t *cur = heap_start;
    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            cur->size += sizeof(block_t) + cur->next->size;
            cur->next  = cur->next->next;
        } else {
            cur = cur->next;
        }
    }
}