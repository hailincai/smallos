#include "gdt.h"

gdt_entry_t gdt[3];
gdt_ptr_t   gdt_ptr;

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

void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 3) - 1;
    gdt_ptr.base  = (u32)&gdt;

    // 1. Null segment
    gdt_set_gate(0, 0, 0, 0, 0);
    // 2. Code segment: Base=0, Limit=4GB, Type=Code/Exec/Read, Ring 0
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    // 3. Data segment: Base=0, Limit=4GB, Type=Data/Read/Write, Ring 0
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // 重新加載 GDTR 並刷新段暫存器
    gdt_flush((u32)&gdt_ptr);
}

void gdt_flush(u32 gdt_ptr_addr) {
__asm__ __volatile__ (
        "lgdt (%0)        \n\t"
        "mov $0x10, %%ax  \n\t"
        "mov %%ax, %%ds   \n\t"
        "mov %%ax, %%es   \n\t"
        "mov %%ax, %%fs   \n\t"
        "mov %%ax, %%gs   \n\t"
        "mov %%ax, %%ss   \n\t"
        "ljmp $0x08, $1f  \n\t"  // 修正點：去掉引用的點，改為 $1f
        "1:               \n\t"  // 修正點：標籤改為 1:
        : 
        : "r" (gdt_ptr_addr)
        : "eax", "memory"        // 建議加上 "memory" 屏障
    );
}