#include "mem.h"
#include "display.h"
#include "isr.h"

u32 page_directory[ENTRIES_COUNT] __attribute__((aligned(4096))) = {0x1};
u32 first_page_table[ENTRIES_COUNT] __attribute__((aligned(4096))) = {0x1};

void init_paging() 
{
    // 初始化頁目錄，將所有入口設置成不存在（屬性0x02)
    for (int i = 0; i < ENTRIES_COUNT; i++)
    {
        page_directory[i] = 0x00000002;
    }

    // 填充第一個頁表，mapping 整個0x00000--0x3ffff (4M空間)
    // 為什麼是4MB，因為每個頁表目錄對應4KB內存，一共1024項
    // 地址的開始就是ith * 4kb, 屬性3 (rw and present)
    for (int i = 0; i < ENTRIES_COUNT; i++)
    {
        first_page_table[i] = (i * PAGE_SIZE) | 0x3;
    }

    // 將第一個頁表放入頁目錄第一項，同時重新設置屬性為0x03
    page_directory[0] = ((u32)first_page_table) | 0x3;

    // register_interrupt_handler(14, page_fault_handler);
}

void enable_paging() {
    // 將頁目錄的位址載入到 CR3 暫存器
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(page_directory));

    // 讀取 CR0，將最高位 (PG bit, 第 31 位) 設為 1
    u32 cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ __volatile__("mov %0, %%cr0" : : "r"(cr0));
}


void page_fault_handler(registers_t *) {
    u32 faulting_address;
    __asm__ __volatile__("mov %%cr2, %0" : "=r"(faulting_address));

    kprint("PAGE FAULT! at virtual address: ");
    kprintf("%x", faulting_address);
    kprint("\nHalting system.");
    while(1)
    {
        __asm__ __volatile__("hlt");
    }
}