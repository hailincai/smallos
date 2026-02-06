#ifndef ISR_H
#define ISR_H
#include "types.h"

#define ISR_MASTER_CONTROL_REG 0x20
#define ISR_SLAVE_CONTROL_REG 0xA0
#define ISR_MASTER_MASK_REG 0x21
#define ISR_SLAVE_MASK_REG 0xA1

typedef struct {
    u32 ds;                                     // 4. 保存的进入中断时候的ds, 对于int 0x80，就是用户数据段
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // 3. 你手動 pusha 的
    u32 int_no, err_code;                       // 2. 你手動 push 的 或 CPU 壓入的
    u32 eip, cs, eflags, useresp, ss;           // 1. CPU 自動壓入的 (最先)
} registers_t;

void register_interrupt_handler(u8 n, void (*handler)(registers_t*));
void init_pic();
#endif