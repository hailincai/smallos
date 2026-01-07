// kernel.c
#include "display.h"
#include "io.h"
#include "types.h"
#include "idt.h"
#include "isr.h"
#include "keyboard.h"
#include "shell.h"
#include "timer.h"

extern void irq0();
extern void irq1();

void main() {
    // all these code are running under PM mode
    clear_screen();
    set_cursor_offset(get_screen_offset(0, 0)); // 初始化游標到左上角

    kprintf("Initializing interrupt table...\n");
    init_pic();
    init_keyboard();
    init_timer(100); // 每秒產生100次中斷
    set_idt_gate(32, (u32)irq0); // 將 IRQ 0 (時鐘) 註冊到 32 號中斷
    set_idt_gate(33, (u32)irq1); // 將 IRQ 1 (鍵盤) 註冊到 33 號中斷
    load_idt();
    __asm__ __volatile__("sti"); // 重要：開啟全域中斷開關. cpu level
    kprint("Loading SmallOS...\n");
    kprint("\nSystem Ready!\n");
    
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