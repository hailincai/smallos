#include "task.h"
#include "util.h" 
#include "gdt.h"
#include "string.h"

// 定義初始內核棧，並強制 16 字節對齊（x86 慣例）
u8 initial_kernel_stack[4096] __attribute__((aligned(16)));

void switch_to_user_mode() {
    // 我們使用 "i" 限制符將 C 語言常量傳入匯編
    __asm__ __volatile__(
        "mov %0, %%ax      \n\t"
        "mov %%ax, %%ds    \n\t"
        "mov %%ax, %%es    \n\t"
        "mov %%ax, %%fs    \n\t"
        "mov %%ax, %%gs    \n\t"

        "push %0           \n\t" // SS (User Data Segment)
        "push %1           \n\t" // ESP (User Stack Top)
        "pushfl            \n\t" // EFLAGS
        
        // 修改棧上的 EFLAGS 開啟中斷 (IF位)
        "pop %%eax         \n\t"
        "or $0x200, %%eax  \n\t" 
        "push %%eax        \n\t"

        "push %2           \n\t" // CS (User Code Segment)
        "push %3           \n\t" // EIP (User Entry Point)
        
        "iret              \n\t"
        : 
        : "i" (USER_DS), 
          "i" (USER_STACK_TOP), 
          "i" (USER_CS), 
          "i" (USER_CODE_VADDR)
        : "ax", "memory"
    );
}
