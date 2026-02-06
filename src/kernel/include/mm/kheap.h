#ifndef KHEAP_H
#define KHEAP_H
#include "types.h"
#include "pmm.h"
#include "vmm.h"
#include <stddef.h>

#define KHEAP_START          0xD0000000
#define KHEAP_INITIAL_SIZE   0x100000  // 初始 1MB
#define KHEAP_MIN_BLOCK_SIZE 16        // 最小分配單位

//保證header也是4字節對齊的
typedef struct kheap_header {
    u32 size;
    u32 is_free;
    struct kheap_header *next;
}kheap_header_t;

typedef struct {
    void *ptr;
    char *label;
} kheap_debug_ptr_t;

void init_kheap();
void *kmalloc(size_t size);
void kfree(void *p);
kheap_header_t *get_kheap_start();
void print_kheap(char *msg, kheap_debug_ptr_t *list, int list_size);

#define DEBUG_KHEAP(msg, ...) \
    do { \
        kheap_debug_ptr_t _list[] = { __VA_ARGS__ }; \
        print_kheap(msg, _list, sizeof(_list) / sizeof(kheap_debug_ptr_t)); \
    } while (0)
#endif