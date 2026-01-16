// kernel.c
#include "display.h"
#include "io.h"
#include "types.h"
#include "idt.h"
#include "isr.h"
#include "keyboard.h"
#include "shell.h"
#include "timer.h"
#include "mem.h"
#include "gdt.h"
#include "pmm.h"
#include "vmm.h"
#include "kheap.h"

extern void isr33();
extern void isr32();
extern void isr14();

void test_pmm_info() {
    u32 total = pmm_get_total_memory() / 1024 / 1024;
    u32 free  = pmm_get_free_memory() / 1024 / 1024;
    u32 used  = pmm_get_used_memory(); // 使用量通常較小，用 byte 或 KB 顯示

    kprintf("Memory Info:\n");
    kprintf("  Total: %x MB\n", total);
    kprintf("  Free: %x MB\n", free);
    kprintf("  Used: %x KB\n", used / 1024);
}

void test_vmm() 
{
    kprintf("Starting VMM test...\n");

    // 1. 分配一個物理頁面
    void* phys_page = pmm_alloc_page();
    u32 test_vaddr = 0x40000000; // 1GB 處的虛擬地址

    kprint("Mapping Phys: "); kprintf("%x", (u32)phys_page);
    kprint(" to Virt: "); kprintf("%x", test_vaddr); kprint("\n");

    // 2. 執行映射
    // 假設 kernel_directory 已經初始化並加載到 CR3
    vmm_map(kernel_directory, test_vaddr, (u32)phys_page, VMM_PAGE_PRESENT | VMM_PAGE_RW);

    // 3. 測試寫入
    // u32* ptr = (u32*)test_vaddr;
    // *ptr = 0xDEADC0DE;

    // 4. 驗證讀取
    // if (*ptr == 0xDEADC0DE) {
    //     kprint("VMM Test Passed! Data matched.\n");
    // } else {
    //     kprint("VMM Test Failed! Data mismatch.\n");
    // }
}

void test_kmalloc() {
    void* p1 = kmalloc(100);
    void* p2 = kmalloc(200);
    kfree(p1);
    void* p3 = kmalloc(50); // 應該佔用原 p1 的前半部分

    DEBUG_KHEAP("Alloc Test", {p2, "p2_buf"}, {p3, "p3_task"});
    // void* p4 = kmalloc(50); // 應該佔用原 p1 的後半部分
    // print_kheap("after allocate p4");

    // kfree(p2);
    // print_kheap("after free p2");   
}

void main() {
    init_gdt();
    
    // all these code are running under PM mode
    clear_screen();
    set_cursor_offset(get_screen_offset(0, 0)); // 初始化游標到左上角

    kprintf("Initializing interrupt table...\n");
    init_pic();
    init_keyboard();
    init_timer(100); // 每秒產生100次中斷
    init_mem();
    set_idt_gate(14, (u32)isr14); // 註冊PAGE FAULT EXCEPTION的中斷wrapper
    set_idt_gate(32, (u32)isr32); // 將 IRQ 0 (時鐘) 註冊到 32 號中斷
    set_idt_gate(33, (u32)isr33); // 將 IRQ 1 (鍵盤) 註冊到 33 號中斷
    load_idt();
    __asm__ __volatile__("sti"); // 重要：開啟全域中斷開關. cpu level
    kprint("Loading SmallOS...\n");

    init_pmm();
    // test_pmm_info();
    init_vmm();
    // test_vmm();
    init_kheap();
    test_kmalloc();    
  
    shell_print_prompt();
    set_backspace_min();
    while(1) 
    {
        if (command_ready == 1)
        {
            shell_execute(key_buffer);
            command_ready = 0;
            key_buffer[0] = '\0';
            shell_print_prompt();
            set_backspace_min();
        }
        __asm__ __volatile__("hlt");
    }
}