#include "idt.h"
#include "util.h"
#include "types.h"

idt_gate_t idt[256];
idt_register_t idt_reg;

void set_idt_gate(int n, u32 handler) {
    idt[n].low_offset = (u16)(handler & 0xFFFF);
    idt[n].sel = 0x08; // 你的內核代碼段選擇子
    idt[n].always0 = 0;
    // 0x8E = 10001110 (Present, Ring 0, 32-bit Interrupt Gate)
    idt[n].flags = 0x8E;
    idt[n].high_offset = (u16)((handler >> 16) & 0xFFFF);
}

void load_idt() {
    idt_reg.base = (u32)&idt;
    idt_reg.limit = 256 * sizeof(idt_gate_t) - 1;
    __asm__ __volatile__("lidt (%0)" : : "r" (&idt_reg));
}