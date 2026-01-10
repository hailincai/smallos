#ifndef PMM_H
#define PMM_H

#include "types.h"
#include "mem.h"

#define PAGE_SIZE 4096
#define PMM_BITMAP_BASE 0xC0100000 // 假設你的內核從這開始，位圖會放在內核結尾後

// 函數聲明
void pmm_init();
void* pmm_alloc_page();
void pmm_free_page(void* addr);

// 用於調試：獲取當前已使用的內存頁數
u32 pmm_get_used_pages();

#endif