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

extern void isr33();
extern void isr32();
extern void isr14();

void main() {
    // all these code are running under PM mode
    clear_screen();
    set_cursor_offset(get_screen_offset(0, 0)); // 初始化游標到左上角

    kprintf("Initializing interrupt table...\n");
    init_pic();
    init_keyboard();
    init_timer(100); // 每秒產生100次中斷
    set_idt_gate(14, (u32)isr14); // 註冊PAGE FAULT EXCEPTION的中斷wrapper
    set_idt_gate(32, (u32)isr32); // 將 IRQ 0 (時鐘) 註冊到 32 號中斷
    set_idt_gate(33, (u32)isr33); // 將 IRQ 1 (鍵盤) 註冊到 33 號中斷
    load_idt();

    // 2. 驗證對齊：地址最後三位 16 進制必須是 000
    kprintf("Validate page directory has correct information...\n");
    kprintf("Page Directory Address: %x\n", (u32)&page_directory);
    
    if (((u32)&page_directory & 0xFFF) != 0) {
        kprint("ERROR: Page directory is NOT 4KB aligned!\n");
        while(1);
    }

    page_directory[0] = 0x12345678;
    if (page_directory[0] != 0x12345678) {
        kprint("DISK LOAD ERROR: Page directory memory not writable/loaded!\n");
        while(1);
    }    

    // --- 在這裡加入分頁初始化 ---
    kprintf("Initializing Paging...\n");
    init_paging();    // 建立頁目錄與頁表
    enable_paging();  // 寫入 CR3 並撥動 CR0 的 PG 位元
    kprintf("Paging enabled...\n");

    __asm__ __volatile__("sti"); // 重要：開啟全域中斷開關. cpu level
    kprint("Loading SmallOS...\n");
    
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