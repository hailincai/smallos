#include "vmm.h"
#include "pmm.h"
#include "mem.h"

// 獲取地址的高 10 位 (PDE 索引)
#define PD_INDEX(vaddr) ((vaddr) >> 22)
// 獲取地址的中間 10 位 (PTE 索引)
// 移除最低的12為（offset), 使用掩碼獲得中間的10位
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x3FF)

// comes from link.ld definition
extern u32 _kernel_end;

// 內核全局頁目錄 (放在高位)
static page_directory_t* kernel_directory = NULL;

void vmm_map(page_directory_t* pd, u32 vaddr, u32 paddr, u32 flags) {
    u32 pd_idx = PD_INDEX(vaddr);
    u32 pt_idx = PT_INDEX(vaddr);

    page_entry_t* pde = &pd->entries[pd_idx];

    // 1. 檢查頁目錄項是否存在 (Present 位)
    if (!(*pde & VMM_PAGE_PRESENT)) {
        // 如果不存在，分配一個物理頁面作為新的頁表
        u32 new_pt_phys = (u32)pmm_alloc_page();
        
        // 將新的頁表掛載到頁目錄 (注意存入的是物理地址)
        // 權限通常給予 Present | RW | User，具體權限由 PTE 控制
        *pde = new_pt_phys | VMM_PAGE_PRESENT | VMM_PAGE_RW;

        // 清空這個新頁表的內容 (需要先轉成虛擬地址才能訪問)
        page_table_t* new_pt_virt = (page_table_t*)PHYS_TO_VIRT(new_pt_phys);
        for (int i = 0; i < ENTRIES_PER_TABLE; i++) {
            new_pt_virt->entries[i] = 0;
        }
    }

    // 2. 找到頁表並設置 PTE
    // pde 的高 20 位是頁表的物理地址
    u32 pt_phys = *pde & ~0xFFF;
    page_table_t* pt_virt = (page_table_t*)PHYS_TO_VIRT(pt_phys);

    // 填入目標物理地址和標誌位
    pt_virt->entries[pt_idx] = (paddr & ~0xFFF) | flags;

    // 3. 通知 CPU 刷新 TLB (如果修改的是當前正在運行的頁表)
    // 簡單的做法是重新加載 CR3，或者使用 invlpg
    __asm__ __volatile__("invlpg (%0)" : : "r" (vaddr) : "memory");
}

// 切換 CR3 的簡單彙編封裝
void vmm_switch_directory(u32 pd_phys) {
    __asm__ __volatile__("mov %0, %%cr3" : : "r" (pd_phys) : "memory");
}

void vmm_init() {
    // 1. 分配頁目錄
    u32 pd_phys = (u32)pmm_alloc_page();
    kernel_directory = (page_directory_t*)PHYS_TO_VIRT(pd_phys);
    
    // 清空頁目錄
    for(int i = 0; i < 1024; i++) kernel_directory->entries[i] = 0;

    // --- A. Identity Map 低端內存 (0 - 4MB) ---
    // 雖然我們可以精確計算，但為了安全（包含 VGA、BIOS 地圖等），
    // 保持前 4MB 的 Identity Map 是一個穩定做法。
    for (u32 addr = 0; addr < 0x400000; addr += PAGE_SIZE) {
        vmm_map(kernel_directory, addr, addr, VMM_PAGE_PRESENT | VMM_PAGE_RW);
    }

    // --- B. 動態映射內核區域 (High Half) ---
    // 物理起始位址是 0x100000 (1MB)
    // 虛擬起始位址是 0xC0100000
    // 結束位址由 &_kernel_end 決定
    
    u32 v_start = 0xC0100000;
    u32 p_start = 0x100000;
    
    // 計算內核（包含位圖）所在的最後一個虛擬位址
    // 這裡我們把位圖也算進去，因為位圖緊跟在 _kernel_end 之後
    // 為了保險，我們可以多映射幾頁，或者直接計算到位圖結束處
    extern u32 bitmap_size_bytes; // 確保這個變量在 pmm.c 中是全局的
    u32 v_end = (u32)&_kernel_end + bitmap_size_bytes;

    for (u32 v_addr = v_start, p_addr = p_start; 
         v_addr < v_end; 
         v_addr += PAGE_SIZE, p_addr += PAGE_SIZE) 
    {
        vmm_map(kernel_directory, v_addr, p_addr, VMM_PAGE_PRESENT | VMM_PAGE_RW);
    }

    // 3. 【手動映射頁目錄本身，防止它落在 4MB 之外
    // 這樣即使 pd_phys 是 100MB 處的地址，內核也能透過虛擬地址訪問它
    vmm_map(kernel_directory, (u32)kernel_directory, pd_phys, VMM_PAGE_PRESENT | VMM_PAGE_RW);    

    // --- C. 切換 CR3 ---
    vmm_switch_directory(pd_phys);
}