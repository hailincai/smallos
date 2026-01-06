#include "isr.h"
#include "idt.h"
#include "display.h"
#include "keyboard.h"

extern void irq1();

void main() {
    clear_screen();
    set_cursor_offset(get_screen_offset(0, 0)); // 初始化游標到左上角
    kprintf("Initializing interrupt table...");
    init_pic();
    init_keyboard();
    set_idt_gate(33, (u32)irq1); // set the keyboard int handler, irq1 comes from asm
    load_idt();
    __asm__ __volatile__("sti");

    kprint("\nSystem Ready!\n");
    while(1)
    {
        __asm__ __volatile__("hlt");
    }
}