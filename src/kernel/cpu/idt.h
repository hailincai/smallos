// src/kernel/idt.h
#ifndef IDT_H
#define IDT_H

#include "types.h" // 確保你已經定義了 u16, u32 等類型

/* IDT 門描述符結構 (Interrupt Gate Descriptor) */
/* 每個條目佔 8 個位元組 */
typedef struct {
    unsigned short low_offset; // 中斷處理程序地址的低 16 位
    unsigned short sel;        // 段選擇子 (通常指向 GDT 中的核心代碼段 0x08)
    unsigned char  always0;    // 必須為 0
    unsigned char  flags;      // 標記位 (呈現位、特權等級、門類型)
    unsigned short high_offset;// 中斷處理程序地址的高 16 位
} __attribute__((packed)) idt_gate_t;

/* IDT 暫存器結構 (IDTR) */
/* 這是傳給 lidt 指令的參數結構 */
typedef struct {
    unsigned short limit;      // IDT 表的總位元組長度 - 1
    unsigned int   base;       // IDT 表在記憶體中的起始絕對地址
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256
extern idt_gate_t idt[IDT_ENTRIES];
extern idt_register_t idt_reg;

/* 函式原型宣告 */
void set_idt_gate(int n, unsigned int handler);
void set_idt_gate_extended(int n, u32 handler, u16 cs_selector, u8 flag);
void load_idt();

#endif