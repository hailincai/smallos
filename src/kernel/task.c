#include "task.h"
#include "util.h" 
#include "gdt.h"
#include "string.h"
#include "vmm.h"
#include "pmm.h"
#include "display.h"

extern char user_program_start[];
extern char user_program_end[];

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

void load_and_start_user_program() {
    u32 program_size = (u32)user_program_end - (u32)user_program_start;

    // 1. 為用戶代碼分配物理頁並映射到 0x400000
    // 注意：如果程序大於 4KB，這裡需要循環分配多個頁
    u32 user_code_phys = (u32)pmm_alloc_page();
    vmm_map(kernel_directory, USER_CODE_VADDR, user_code_phys, 0x07); // User, RW, Present

    // 2. 將嵌入的 init.bin 拷貝到剛映射的物理頁
    // 我們使用 PHYS_TO_VIRT 訪問該物理頁，以便在內核態進行寫入
    k_mem_copy((void*)PHYS_TO_VIRT(user_code_phys), (void*)user_program_start, program_size);

    // 3. 為用戶棧分配物理頁並映射到 0x800000
    u32 user_stack_phys = (u32)pmm_alloc_page();
    vmm_map(kernel_directory, USER_STACK_VADDR, user_stack_phys, 0x07);

    kprintf("User program loaded. Size: %d bytes. Jumping to Ring 3...\n", program_size);

    // 4. 正式跳轉
    switch_to_user_mode();
}