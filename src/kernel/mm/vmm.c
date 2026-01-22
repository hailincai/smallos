#include "vmm.h"
#include "pmm.h"
#include "mem.h"
#include "gdt.h"
#include "idt.h"
#include "display.h"

// 獲取地址的高 10 位 (PDE 索引)
#define PD_INDEX(vaddr) ((vaddr) >> 22)
// 獲取地址的中間 10 位 (PTE 索引)
// 移除最低的12為（offset), 使用掩碼獲得中間的10位
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x3FF)

// comes from link.ld definition
extern u32 _kernel_end;

// 內核全局頁目錄 (放在高位)
page_directory_t* kernel_directory = NULL;

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
        // PDE 應該讓內核態和用戶態都能訪問
        // PTE會正式區分是內核態還是用戶態，從而實現權限控制
        *pde = new_pt_phys | VMM_PAGE_PRESENT | VMM_PAGE_RW | VMM_PAGE_USER;

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
    // 告訴 CPU 某個特定的虛擬地址映射已經失效，請將其從 TLB 中清除
    __asm__ __volatile__("invlpg (%0)" : : "r" (vaddr) : "memory");
}

// 切換 CR3 的簡單彙編封裝
void vmm_switch_directory(u32 pd_phys) {
    __asm__ __volatile__("mov %0, %%cr3" : : "r" (pd_phys) : "memory");
}

void init_vmm() {
// 1. 進入關鍵區：關閉中斷，防止切換期間發生異常導致 Triple Fault
    __asm__ __volatile__("cli");

    // 2. 分配物理頁面作為頁目錄 (Page Directory)
    u32 pd_phys = (u32)pmm_alloc_page();
    
    // 獲取其虛擬地址以便內核訪問
    kernel_directory = (page_directory_t*)PHYS_TO_VIRT(pd_phys);

    // 3. 初始化頁目錄：將所有條目設為「不存在」
    for (int i = 0; i < ENTRIES_PER_TABLE; i++) {
        kernel_directory->entries[i] = 0;
    }

    // 4. Identity Map 低端內存 (0 - 4MB)
    // 確保在切換 CR3 及其後的幾行代碼執行時，EIP 仍然指向有效地址
    // 而且我们没有给他设置VM_PAGE_USER, 所以0x00000000 and 0xC0000000开始的4MB都是用户态的禁区
    for (u32 addr = 0; addr < 0x400000; addr += PAGE_SIZE) {
        vmm_map(kernel_directory, addr, addr, VMM_PAGE_PRESENT | VMM_PAGE_RW);
    }

    // 5. 映射 High Half 內核空間 (0xC0000000 - 0xC0400000)
    // 這裡直接映射完整的 4MB 區域，確保覆蓋內核代碼、數據、BSS、GDT/IDT 和 PMM 位圖
    for (u32 i = 0; i < 0x400000; i += PAGE_SIZE) {
        vmm_map(kernel_directory, 0xC0000000 + i, i, VMM_PAGE_PRESENT | VMM_PAGE_RW);
    }

    // 6. 手動映射頁目錄自身
    // 這是為了確保當前正在使用的 Page Directory 在分頁開啟後依然可以被訪問
    vmm_map(kernel_directory, (u32)kernel_directory, pd_phys, VMM_PAGE_PRESENT | VMM_PAGE_RW);

    // 7. 切換 CR3 暫存器，正式啟用新的分頁結構
    vmm_switch_directory(pd_phys);

    // 8. 刷新環境：在新的虛擬位址空間重新加載 GDT 和 IDT
    // 這一點至關重要，因為舊的指針可能指向了未映射或不正確的物理地址
    init_gdt();
    load_idt();

    // 9. 恢復中斷處理
    __asm__ __volatile__("sti");

    kprint("VMM enabled and environment reloaded successfully.\n");
}