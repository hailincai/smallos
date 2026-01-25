#include "idt.h"
#include "util.h"
#include "types.h"
#include "gdt.h"

idt_gate_t idt[256];
idt_register_t idt_reg;

void set_idt_gate(int n, u32 handler) {
    // 0x08 kernel code segemnt
    // 0x8E = 10001110 (Present, Ring 0, 32-bit Interrupt Gate)
    set_idt_gate_extended(n, handler, KERNEL_CS, 0x8E);
}

void set_idt_gate_extended(int n, u32 handler, u16 cs_selector, u8 flag)
{
    idt[n].low_offset = (u16)(handler & 0xFFFF);
    idt[n].sel = cs_selector; // 你的內核代碼段選擇子
    idt[n].always0 = 0;
    // 0x8E = 10001110 (Present, Ring 0, 32-bit Interrupt Gate)
    idt[n].flags = flag;
    idt[n].high_offset = (u16)((handler >> 16) & 0xFFFF);    
}

void load_idt() {
    idt_reg.base = (u32)&idt;
    idt_reg.limit = 256 * sizeof(idt_gate_t) - 1;
    __asm__ __volatile__("lidt (%0)" : : "r" (&idt_reg));
}