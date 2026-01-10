#ifndef MEM_H
#define MEM_H
#include "types.h"
#include "isr.h"

/* 內核基址定義 */
#define KERNEL_VIRT_BASE 0xC0000000

/* 地址轉換工具 */
#define PHYS_TO_VIRT(addr) ((void*)((u32)(addr) + KERNEL_VIRT_BASE))
#define VIRT_TO_PHYS(addr) ((void*)((u32)(addr) - KERNEL_VIRT_BASE))

/* 常用設備映射地址 */
#define VIDEO_VIRT_ADDR  PHYS_TO_VIRT(0xB8000)

// this is the memory map entry structure
// return by BIOS call
typedef struct {
    u32 base_low;
    u32 base_high;
    u32 length_low;
    u32 length_high;
    u32 type;
    u32 acpi;    // 有些 BIOS 只返回 20 字節，有些 24，建議保留
} __attribute__((packed)) mmap_entry_t;

void init_mem();
void page_fault_handler(registers_t *regs);
#endif