#include "pmm.h"

// 外部符號，由 linker script 提供
extern u32 _kernel_end;

static u32* bitmap;
static u32 total_pages = 0;
static u32 used_pages = 0;
static u32 bitmap_size_bytes = 0;

// 內部輔助：設置位圖中的某一位
// divide 32 因為每個bitmap可以表示32個page的狀態
static void bitmap_set(u32 page_idx) {
    bitmap[page_idx / 32] |= (1 << (page_idx % 32));
    // 確保這一點的修改在之後的代碼讀取前已經寫入內存
    COMPILER_BARRIER();    
}

// 內部輔助：清除位圖中的某一位
static void bitmap_unset(u32 page_idx) {
    bitmap[page_idx / 32] &= ~(1 << (page_idx % 32));
    // 確保這一點的修改在之後的代碼讀取前已經寫入內存
    COMPILER_BARRIER();    
}

// 內部輔助：測試位圖中的某一位是否為 1
static int bitmap_test(u32 page_idx) {
    return (bitmap[page_idx / 32] & (1 << (page_idx % 32)));
}

void pmm_init() {
    u32 entry_count = *(u32*)0xC0008000; // PHYS_TO_VIRT(0x8000)
    mmap_entry_t* entries = (mmap_entry_t*)0xC0008004;

    u32 max_addr = 0;

    // 1. 找到最大可訪問物理位址來決定位圖大小
    for (u32 i = 0; i < entry_count; i++) {
        if (entries[i].type == 1) { // Available
            u32 end_addr = entries[i].base_low + entries[i].length_low;
            if (end_addr > max_addr) max_addr = end_addr;
        }
    }

    // 每個page 4k, 所以total pages = max_addr/PAGE_SIZE
    total_pages = max_addr / PAGE_SIZE; 
    // 每個page使用一個bit來表示，所以我們一共需要total_pages / 8 bytes
    bitmap_size_bytes = total_pages / 8; // 每個 bit 代表一頁，8 bits 為一字節
    
    // 2. 將位圖放在內核結束後的地址 (高位虛擬地址)
    bitmap = (u32*)&_kernel_end;

    // 3. 初始化位圖：預設全部標記為「已使用」(1)
    // 這樣我們只需要把 Available 的區域「開放」出來
    for (u32 i = 0; i < bitmap_size_bytes / 4; i++) {
        bitmap[i] = 0xFFFFFFFF;
    }

    // 4. 根據內存地圖開放可用區域
    for (u32 i = 0; i < entry_count; i++) {
        if (entries[i].type == 1) {
            u32 page_start = entries[i].base_low / PAGE_SIZE;
            u32 page_count = entries[i].length_low / PAGE_SIZE;
            for (u32 j = 0; j < page_count; j++) {
                bitmap_unset(page_start + j);
            }
        }
    }

    // 5. 重新鎖定敏感區域
    // A. 鎖定物理 1MB 以下 (包含地圖本身和 BIOS 區域)
    for (u32 i = 0; i < 256; i++) bitmap_set(i); 

    // B. 鎖定內核代碼區和位圖本身
    u32 kernel_phys_start = 0x100000; // 1MB
    u32 kernel_phys_end = ((u32)bitmap + bitmap_size_bytes - 0xC0000000);
    u32 k_start_page = kernel_phys_start / PAGE_SIZE;
    u32 k_end_page = (kernel_phys_end / PAGE_SIZE) + 1;

    for (u32 i = k_start_page; i < k_end_page; i++) {
        bitmap_set(i);
    }

    used_pages = 0;
    // 注意：total_pages 可能不是 32 的整數倍，但 bitmap 是以 u32 為單位的
    // 這裡我們直接遍歷所有 page 索引來統計最安全
    for (u32 i = 0; i < total_pages; i++) {
        if (bitmap_test(i)) {
            used_pages++;
        }
    }    
}

void* pmm_alloc_page() {
    for (u32 i = 0; i < total_pages / 32; i++) {
        if (bitmap[i] != 0xFFFFFFFF) { // 這 32 頁裡至少有一頁是空的
            for (int j = 0; j < 32; j++) {
                if (!(bitmap[i] & (1 << j))) {
                    u32 page_idx = i * 32 + j;
                    bitmap_set(page_idx);
                    used_pages++;
                    return (void*)(page_idx * PAGE_SIZE);
                }
            }
        }
    }
    return NULL; // OOM
}

void pmm_free_page(void* addr) {
    u32 page_idx = (u32)addr / PAGE_SIZE;
    bitmap_unset(page_idx);
    used_pages--;
}

u32 pmm_get_free_memory() {
    return (total_pages - used_pages) * PAGE_SIZE;
}

u32 pmm_get_used_memory() {
    return used_pages * PAGE_SIZE;
}

u32 pmm_get_total_memory() {
    return total_pages * PAGE_SIZE;
}