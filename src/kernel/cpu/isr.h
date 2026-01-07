#ifndef ISR_H
#define ISR_H
#include "types.h"

#define ISR_MASTER_CONTROL_REG 0x20
#define ISR_SLAVE_CONTROL_REG 0xA0
#define ISR_MASTER_MASK_REG 0x21
#define ISR_SLAVE_MASK_REG 0xA1

void register_interrupt_handler(u8 n, void (*handler)(u32));
void init_pic();
#endif