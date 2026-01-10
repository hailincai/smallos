#ifndef GDT_H
#define GDT_H

#include "types.h"

// 定義 GDT 描述符項
struct gdt_entry_struct {
    u16 limit_low;     // 段限長 (0-15 bits)
    u16 base_low;      // 段基址 (0-15 bits)
    u8  base_middle;   // 段基址 (16-23 bits)
    u8  access;        // 存取字 (Access Byte)
    u8  granularity;   // 粒度與段限長高位 (Flags + Limit 16-19)
    u8  base_high;     // 段基址 (24-31 bits)
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

// GDTR 暫存器結構
struct gdt_ptr_struct {
    u16 limit;         // GDT 大小 - 1
    u32 base;          // GDT 的虛擬起始位址
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

// 導出函數
void init_gdt();
void gdt_flush(u32); // 由彙編實現
void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran);
#endif