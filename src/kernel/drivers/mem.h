#ifndef MEM_H
#define MEM_H
#include "types.h"
#include "isr.h"

void init_mem();
void page_fault_handler(registers_t *regs);
#endif