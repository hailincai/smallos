[bits 32]
global _start
extern main

section .text
_start:
    ; --- 調試：在螢幕左上角印出一個白底紅字的 'K' ---
    mov word [0xb8000], 0x4f4b ; 0x4f = 白底紅字, 0x4b = 'K'
    ; 1. 建立簡單的 Identity Mapping (0-4MB)
    ; 將頁表項填滿
    mov edi, boot_page_table
    mov eax, 0x00000003          ; 物理 0, 屬性: P+W
    mov ecx, 1024
.fill_pt:
    stosd
    add eax, 4096
    loop .fill_pt

    ; 2. 設置頁目錄
    mov eax, boot_page_table
    or eax, 0x00000003
    mov [boot_page_directory], eax
    
    ; --- 調試：印出 'P' 代表頁表初始化完成 ---
    mov word [0xb8002], 0x4f50 ; 'P'    

    ; 3. 開啟分頁
    mov eax, boot_page_directory
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; --- 調試：印出 'M' 代表分頁開啟成功，且 EIP 依然正常 ---
    mov word [0xb8004], 0x4f4d ; 'M'

    ; 4. 直接跳轉到 C 語言 (因為現在 VMA == LMA)
    mov esp, stack_top
    call main
    jmp $

section .data
align 4096
boot_page_directory:
    times 1024 dd 0
boot_page_table:
    times 1024 dd 0

section .bss
align 16
stack_bottom:
    resb 16384     ; 預留 16KB，NASM 不會報警告
stack_top: