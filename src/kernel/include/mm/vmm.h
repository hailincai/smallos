#ifndef VMM_H
#define VMM_H

#include "types.h"

// 頁表項屬性位元定義
#define VMM_PAGE_PRESENT   0x1         // 在內存中
#define VMM_PAGE_RW        0x2         // 讀寫權限 (0: 只讀, 1: 讀寫)
#define VMM_PAGE_USER      0x4         // 用戶級別 (0: 內核, 1: 用戶)
#define VMM_PAGE_PWT       0x8         // Write-through
#define VMM_PAGE_PCD       0x10        // Cache disabled
#define VMM_PAGE_ACCESSED  0x20        // 是否被訪問過
#define VMM_PAGE_DIRTY     0x40        // 是否被寫入過 (僅對 PTE 有效)
#define VMM_PAGE_4MB       0x80        // 是否開啟 4MB 大頁 (僅對 PDE 有效)

// 定義一個頁表項 (Page Table Entry) 或 頁目錄項 (Page Directory Entry)
// 本質上它們都是 32 位元的數據
typedef u32 page_entry_t;

// 每個頁表/頁目錄包含 1024 個條目
#define ENTRIES_PER_TABLE 1024
#define PAGE_SIZE         4096

typedef struct page_directory {
    page_entry_t entries[ENTRIES_PER_TABLE];
} __attribute__((aligned(PAGE_SIZE))) page_directory_t;

typedef struct page_table {
    page_entry_t entries[ENTRIES_PER_TABLE];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

extern page_directory_t* kernel_directory;

// 初始化 VMM
void init_vmm();

// 將虛擬地址映射到物理地址
void vmm_map(page_directory_t* pd, u32 vaddr, u32 paddr, u32 flags);

// 切換頁目錄 (修改 CR3)
void vmm_switch_directory(u32 pd);

#endif