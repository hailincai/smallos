[bits 32]
section .text
    global _start
    extern main

; 定義常量
VM_BASE     equ 0xC0000000                  ; 虛擬地址基址
PDE_INDEX   equ (VM_BASE >> 22)             ; 0xC0000000 對應第 768 個頁目錄項

_start:
    ; 1. 獲取頁目錄和頁表的物理地址
    ; 因為此時分頁未開，標籤地址都在 0xC01xxxxx，必須減去偏移
    mov edi, (boot_page_directory - VM_BASE)
    mov esi, (boot_page_table - VM_BASE)

    ; 2. 填充臨時頁表 (boot_page_table)
    ; 我們映射物理地址 0x000000 到 0x400000 (前 4MB)
    mov eax, 0x00000003                     ; 物理地址 0, 屬性: Present + Writable
    mov ecx, 1024                           ; 填充 1024 個頁表項
.fill_table:
    mov [esi], eax
    add eax, 4096                           ; 下一個物理頁
    add esi, 4                              ; 下一個頁表項
    loop .fill_table

    ; 3. 設置頁目錄 (boot_page_directory)
    ; Identity Mapping: 將 0~4MB 虛擬地址映射到 0~4MB 物理地址
    mov eax, (boot_page_table - VM_BASE)
    or eax, 0x00000003                      ; 屬性: Present + Writable
    mov edi, (boot_page_directory - VM_BASE)
    mov [edi], eax                          ; 映射第 0 個 PDE (0~4MB)

    ; High Half Mapping: 將 3GB~3GB+4MB 映射到 0~4MB 物理地址
    mov [edi + PDE_INDEX * 4], eax          ; 映射第 768 個 PDE (3GB~3GB+4MB)

    ; 4. 加載頁目錄到 CR3
    mov eax, edi
    mov cr3, eax

    ; 5. 開啟分頁 (設置 CR0 的 PG 位)
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; 6. 強制跳轉到高位地址 (Higher Half)
    ; 這一跳之後，EIP 進入 0xC01xxxxx 區間
    lea eax, [higher_half]
    jmp eax

higher_half:
    ; 此時已在虛擬地址運行，可以直接存取符號
    mov esp, stack_top                      ; 設置高位堆疊
    mov ebp, esp

    ; 清除 Identity Mapping (可選，但為了安全建議移除第 0 個 PDE)
    ; mov dword [boot_page_directory], 0
    ; invlpg [0]

    call main
    jmp $

; ---------------------------------------------------------
section .bss
align 4096
boot_page_directory:
    resb 4096
boot_page_table:
    resb 4096

stack_bottom:
    resb 16384
stack_top: