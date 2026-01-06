#ifndef ISR_H
#define ISR_H
#include "types.h"
void register_interrupt_handler(u8 n, void (*handler)(u32));
void init_pic();
#endif