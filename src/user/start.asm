; user/start.s
[bits 32]
[extern main]
[global _start]

_start:
    call main      ; 調用用戶態的 C 代碼
.halt:
    jmp .halt      ; 如果 main 返回，進入死循環（以後這裡改為 exit syscall）