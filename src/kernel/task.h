#ifndef TASK_H
#define TASK_H
#include "types.h"

// 用戶態初始空間定義
#define USER_STACK_TOP    0x801000
#define USER_CODE_VADDR   0x400000

extern u8 initial_kernel_stack[4096];
void switch_to_user_mode();
#endif