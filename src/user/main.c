#include "syscall.h"

void main() {
    // // 定義要打印的字符串
    // // 注意：這個字符串位於用戶態的數據段（0x400000 附近）
    // char *msg = "Hello, GeminiOS Syscall!";

    // // 發起系統調用
    // // eax = 1 (系統調用號)
    // // ebx = msg (參數 1)
    // __asm__ __volatile__ (
    //     "mov $0, %%eax;"
    //     "mov %0, %%ebx;"
    //     "int $0x80"
    //     : 
    //     : "r"(msg)
    //     : "eax", "ebx"
    // );

    syscall(1, (int)"Hello world from user!");

    // 系統調用返回後，進入死循環
    while(1);
}