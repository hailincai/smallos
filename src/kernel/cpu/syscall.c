#include "syscall.h"
#include "isr.h"
#include "display.h"

// 系統調用函數原型
void sys_print(registers_t *regs) {
    char *user_msg = (char *)regs->ebx;
    kprintf("[User]: %s\n", user_msg);
}

// 系統調用表
static void *syscall_table[MAX_SYSCALLS] = {
    [1] = sys_print
};

void syscall_dispatch(registers_t *regs)
{
    if (regs->eax >= MAX_SYSCALLS)
    {
        kprintf("Unknown syscall: %d\n", regs->eax);
        return;
    }

    void (*handler)(registers_t*) = syscall_table[regs->eax];
    if (handler) {
        handler(regs);
    }
}

void init_syscall()
{
    register_interrupt_handler(128, syscall_dispatch);
}

