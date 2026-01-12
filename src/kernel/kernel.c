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

    pmm_init();
    test_pmm_info();
    
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