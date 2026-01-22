#include "gdt.h"
#include "string.h"
#include "util.h"

gdt_entry_t gdt[6];
gdt_ptr_t   gdt_ptr;
tss_entry_t tss_entry;

// 設置 GDT 描述符的輔助函數
void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;

    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void write_tss(s32 num, u16 ss0, u32 esp0) {
    // 1. 獲取 tss_entry 的基址和限長
    u32 base = (u32)&tss_entry;
    u32 limit = sizeof(tss_entry) - 1;

    // 2. 在 GDT 中註冊 TSS 描述符
    // Access Byte: 0x89 (10001001) -> P=1, DPL=0, Type=Available 386 TSS
    // 注意：TSS 描述符的 DPL 通常設為 0
    gdt_set_gate(num, base, limit, 0x89, 0x00);

    // 3. 清空 TSS 結構體
    k_mem_set(&tss_entry, 0, sizeof(tss_entry));

    // 4. 設置關鍵字段
    tss_entry.ss0 = ss0;   // 內核數據段選擇子 (通常是 0x10)
    tss_entry.esp0 = esp0; // 初始內核棧頂 (之後切換進程時會動態更新)

    // 設置 IO 位圖基址，設為 sizeof(tss) 代表沒有 IO 位圖
    tss_entry.iomap_base = sizeof(tss_entry);
}

void tss_flush() {
    // 0x28 是 Index 5 (5 * 8 = 40 = 0x28)
    // 我們加載的是選擇子
    __asm__ __volatile__("ltr %%ax" : : "a" (TSS_SEL));
}

void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * ARRAY_SIZE(gdt)) - 1;
    gdt_ptr.base  = (u32)&gdt;

    // 1. Null segment
    gdt_set_gate(0, 0, 0, 0, 0);
    // 2. Code segment: Base=0, Limit=4GB, Type=Code/Exec/Read, Ring 0
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    // 3. Data segment: Base=0, Limit=4GB, Type=Data/Read/Write, Ring 0
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    // 4. User Code: Base=0, Limit=4GB, Ring 3 (Access=0xFA)
    // 0xFA = 1(P) 11(DPL) 1(S) 1010(Type: Exec/Read)
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    // 5. User Data: Base=0, Limit=4GB, Ring 3 (Access=0xF2)
    // 0xF2 = 1(P) 11(DPL) 1(S) 0010(Type: Read/Write)
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);   
    
    // 6. TSS: Base=&tss_entry, Limit=sizeof(tss), Ring 0 (Access=0x89)
    // 注意：TSS 的初始化需要單獨的函數，這裡先留出索引 5 的位置
    write_tss(5, KERNEL_DS, 0); // 稍後調用    

    // 重新加載 GDTR 並刷新段暫存器
    gdt_flush((u32)&gdt_ptr);

    tss_flush();
}

void gdt_flush(u32 gdt_ptr_addr) {
    // memory means
    // 阻止編譯器優化任何和相關內存的代碼
    // 並且保證在這個代碼執行之後，任何對相關內存的訪問都會重新read, 不會使用任何的cache
    __asm__ __volatile__ (
            "lgdt (%0)            \n\t"
            "mov %1, %%ax         \n\t" // 使用傳入的 KERNEL_DS
            "mov %%ax, %%ds       \n\t"
            "mov %%ax, %%es       \n\t"
            "mov %%ax, %%fs       \n\t"
            "mov %%ax, %%gs       \n\t"
            "mov %%ax, %%ss       \n\t"
            "push %2              \n\t" // 將 KERNEL_CS 壓棧
            "push $1f             \n\t" // 將目標偏移壓棧
            "lret                 \n\t" // 使用 lret 實現長跳轉 (Far Return)
            "1:                   \n\t"
            : 
            : "r" (gdt_ptr_addr), 
            "i" (KERNEL_DS), 
            "i" (KERNEL_CS)
            : "eax", "memory"
        );
}

// 這樣當用戶程序發生中斷時，CPU 才知道要把寄存器壓到哪個內核棧裡
void set_kernel_stack(u32 stack) {
    tss_entry.esp0 = stack;
}