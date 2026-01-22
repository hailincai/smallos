#ifndef GDT_H
#define GDT_H

#include "types.h"

#define KERNEL_CS 0x08  // (1 * 8)
#define KERNEL_DS 0x10  // (2 * 8)
#define USER_CS   0x1B  // (3 * 8) | 3 (RPL)
#define USER_DS   0x23  // (4 * 8) | 3 (RPL)
#define TSS_SEL   0x28  // (5 * 8)

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

typedef struct tss_entry_struct {
    u32 prev_tss;   // 上一個任務的 TSS
    u32 esp0;       // Ring 3 -> Ring 0 切換時加載的堆棧指針
    u32 ss0;        // Ring 3 -> Ring 0 時加載的堆棧段 (通常是 0x10)
    u32 esp1, ss1, esp2, ss2, cr3, eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    u32 es, cs, ss, ds, fs, gs, ldt;
    u16 trap, iomap_base;
} __attribute__((packed)) tss_entry_t;

// 導出函數
void init_gdt();
void gdt_flush(u32); // 由彙編實現
void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran);
void set_kernel_stack(u32 stack);
#endif