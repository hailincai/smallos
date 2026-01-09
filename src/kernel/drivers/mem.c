#include "mem.h"
#include "display.h"
#include "isr.h"

void init_mem() 
{
    register_interrupt_handler(14, page_fault_handler);
}

void page_fault_handler(registers_t *) {
    u32 faulting_address;
    __asm__ __volatile__("mov %%cr2, %0" : "=r"(faulting_address));

    kprint("PAGE FAULT! at virtual address: ");
    kprintf("%x", faulting_address);
    kprint("\nHalting system.");
    while(1)
    {
        __asm__ __volatile__("hlt");
    }
}