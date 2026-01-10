#include "mem.h"
#include "display.h"
#include "isr.h"

char *mem_map_entry_type(int type)
{
        switch (type) {
            case 1: return "Available";
            case 2: return "Reserved ";
            case 3: return "ACPI Rec ";
            default: return "Unknown  ";
        }
}

void display_memory_map() {
    // 1. 讀取條目數量（存放在 0x8000 物理位址）
    u32 entry_count = *(u32*)PHYS_TO_VIRT(0x8000);
    // 2. 條目從 0x8004 開始
    mmap_entry_t* entries = (mmap_entry_t*)PHYS_TO_VIRT(0x8004);

    kprint("--- Memory Map Start ---\n");
    
    for (u32 i = 0; i < entry_count; i++) {
        // 為了簡單，我們先處理 32 位地址，如果是 64 位內存則需要處理 base_high
        kprintf("Base: %x  | Length: %x | Type: %s\n"
            , entries[i].base_low
            , entries[i].length_low
            , mem_map_entry_type(entries[i].type));        
    }
    
    kprint("--- Memory Map End ---\n");
}

void init_mem() 
{
    display_memory_map();
    register_interrupt_handler(14, page_fault_handler);
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