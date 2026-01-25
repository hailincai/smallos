#ifndef USR_SYSCALL_H
#define USR_SYSCALL_H
static inline int syscall(int num, int arg1) {
    int ret;
    __asm__ __volatile__ (
        "int $0x80"
        : "=a"(ret)             // 返回值存於 eax
        : "a"(num), "b"(arg1)   // 編號存於 eax, 參數存於 ebx
    );
    return ret;
}
#endif