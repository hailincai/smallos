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
#include "task.h"
#include "string.h"

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

void user_test_app() {
    while(1) {
        // 这里不能 call kprintf，只能做计算或死循环
    }
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

    init_pmm();
    // test_pmm_info();
    init_vmm();
    // test_vmm();
    init_kheap();
    // test_kmalloc();    
  
    // shell_print_prompt();
    // set_backspace_min();
    // while(1) 
    // {
    //     if (command_ready == 1)
    //     {
    //         shell_execute(key_buffer);
    //         command_ready = 0;
    //         key_buffer[0] = '\0';
    //         shell_print_prompt();
    //         set_backspace_min();
    //     }
    //     __asm__ __volatile__("hlt");
    // }

    // temporary disable the interrupt, 
    // so the esp switch will not cause any issue with interrupt handler
    __asm__ __volatile__("cli");

    // 更新 TSS 里的内核栈顶，这决定了中断发生时跳回哪里
    // 必须指向我们刚才新定义的 initial_kernel_stack 顶部
    set_kernel_stack((u32)initial_kernel_stack + 4096);

    // --- 【新增：用户态准备】 ---
    kprintf("Preparing User Mode at 0x400000...\n");
    
    // 1. 分配并映射用户代码页 (0x400000)
    u32 user_code_phys = (u32)pmm_alloc_page();
    vmm_map(kernel_directory, 0x400000, user_code_phys, 0x07); // User | RW | Present
    
    // 2. 拷贝代码（将 user_test_app 拷贝到物理页对应的内核虚拟地址）
    k_mem_copy((void*)PHYS_TO_VIRT(user_code_phys), (void*)user_test_app, 1024);

    // 3. 分配并映射用户栈页 (0x800000)
    u32 user_stack_phys = (u32)pmm_alloc_page();
    vmm_map(kernel_directory, 0x800000, user_stack_phys, 0x07);

    // --- 【执行跳转】 ---
    kprintf("Entering User Mode...\n");
    switch_to_user_mode(); // 这里执行 iret 逻辑

    // 正常情况下，switch_to_user_mode 不会返回到这里
    while(1) { __asm__ __volatile__("hlt"); }    
}